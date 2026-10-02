// Tests for MinHeap.
// The key idea behind every test here: we cannot see inside the heap (data_ is
// private), so we test it the way it will actually be used - push things in,
// pop them out, and check they come out in the right order. If sift-up or
// sift-down is wrong anywhere, some value will come out too early.

#include "MinHeap.hpp"
#include "test_util.hpp"

#include <stdexcept>
#include <string>

using namespace opera;

// A new heap is empty.
static void testStartsEmpty() {
    MinHeap<int> h;
    CHECK(h.empty());
    CHECK_EQ(h.size(), 0u);
}

// top() always shows the smallest value, after every single push.
static void testTopTracksMinimum() {
    MinHeap<int> h;

    h.push(50);  CHECK_EQ(h.top(), 50);
    h.push(30);  CHECK_EQ(h.top(), 30);   // new minimum must rise to the top
    h.push(40);  CHECK_EQ(h.top(), 30);   // not a new minimum, top unchanged
    h.push(10);  CHECK_EQ(h.top(), 10);
    h.push(20);  CHECK_EQ(h.top(), 10);

    CHECK_EQ(h.size(), 5u);
}


// Popping everything gives values in exact ascending order.
// Inputs are deliberately out of order and include a duplicate.

static void testPopsInAscendingOrder() {
    MinHeap<int> h;
    for (int v : {42, 7, 19, 3, 88, 3, 56, 1}) {
        h.push(v);
    }

    // Expected answer worked out by hand: the inputs sorted.
    const int expected[] = {1, 3, 3, 7, 19, 42, 56, 88};
    for (int want : expected) {
        CHECK_EQ(h.pop(), want);
    }
    CHECK(h.empty());
}

// Stress test: 1000 pseudo-random values with lots of duplicates.

static void testStressRandomOrder() {
    MinHeap<int> h;

    //fixed-seed generator, so the test gives the same result every
    // run.
    unsigned seed = 12345u;
    const int count = 1000;
    for (int i = 0; i < count; ++i) {
        seed = seed * 1103515245u + 12345u;
        h.push(static_cast<int>((seed / 65536u) % 100u));   // values 0..99
    }
    CHECK_EQ(h.size(), static_cast<std::size_t>(count));

    int  previous = h.pop();
    int  popped   = 1;
    bool inOrder  = true;
    while (!h.empty()) {
        const int current = h.pop();
        if (current < previous) {
            inOrder = false;
        }
        previous = current;
        ++popped;
    }
    CHECK(inOrder);
    CHECK_EQ(popped, count);
}

// Mixing pushes and pops. Real use (Dijkstra, triage) never pushes everything
// first and then pops everything - it interleaves them.
static void testInterleavedPushAndPop() {
    MinHeap<int> h;
    h.push(5);
    h.push(2);
    CHECK_EQ(h.pop(), 2);

    h.push(8);
    h.push(1);
    CHECK_EQ(h.pop(), 1);
    CHECK_EQ(h.pop(), 5);

    h.push(3);
    CHECK_EQ(h.pop(), 3);
    CHECK_EQ(h.pop(), 8);
    CHECK(h.empty());
}

// Swapping the comparator turns it into a MAX-heap.
// The triage queue will rely on this to put the most severe case on top.
struct GreaterThan {
    bool operator()(int a, int b) const { return a > b; }
};

static void testComparatorMakesMaxHeap() {
    MinHeap<int, GreaterThan> h;
    for (int v : {5, 50, 15, 1, 30}) {
        h.push(v);
    }
    CHECK_EQ(h.pop(), 50);
    CHECK_EQ(h.pop(), 30);
    CHECK_EQ(h.pop(), 15);
    CHECK_EQ(h.pop(), 5);
    CHECK_EQ(h.pop(), 1);
}

// Heap of structs, ordered by one field. This is exactly how Dijkstra will use
// it: each item is (distance, location), ordered by distance.

struct Item {
    double      dist;
    std::string place;
};

struct CloserFirst {
    bool operator()(const Item& a, const Item& b) const { return a.dist < b.dist; }
};

static void testStructsWithCustomOrdering() {
    MinHeap<Item, CloserFirst> h;
    h.push({9.5, "Riverside"});
    h.push({2.0, "Market"});
    h.push({4.5, "Station"});

    CHECK(h.top().place == "Market");
    CHECK(h.pop().place == "Market");
    CHECK(h.pop().place == "Station");

    const Item last = h.pop();
    CHECK(last.place == "Riverside");
    CHECK_NEAR(last.dist, 9.5, 1e-9);
}

// accessing top() or pop() on an empty heap throws std::out_of_range. This is
// important because the triage queue will be empty sometimes, and Dijkstra will
// be empty when the destination is unreachable. The caller must be able to
// detect that and handle it gracefully, rather than crashing the program.
static void testEmptyHeapThrows() {
    MinHeap<int> h;

    bool topThrew = false;
    try {
        (void)h.top();
    } catch (const std::out_of_range&) {
        topThrew = true;
    }
    CHECK(topThrew);

    bool popThrew = false;
    try {
        (void)h.pop();
    } catch (const std::out_of_range&) {
        popThrew = true;
    }
    CHECK(popThrew);

    // A heap emptied by popping must behave the same as a fresh one.
    h.push(1);
    (void)h.pop();
    CHECK(h.empty());
    bool threwAgain = false;
    try {
        (void)h.pop();
    } catch (const std::out_of_range&) {
        threwAgain = true;
    }
    CHECK(threwAgain);
}

int main() {
    testStartsEmpty();
    testTopTracksMinimum();
    testPopsInAscendingOrder();
    testStressRandomOrder();
    testInterleavedPushAndPop();
    testComparatorMakesMaxHeap();
    testStructsWithCustomOrdering();
    testEmptyHeapThrows();

    return report("test_heap");
}