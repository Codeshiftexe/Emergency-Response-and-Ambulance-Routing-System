#pragma once

// A binary min-heap.
// This is the priority queue for the whole project. Two places will use it:
//   - Dijkstra, to always expand the closest unvisited location next
//   - the triage queue, to always serve the most urgent emergency next

#include <cstddef>
#include <stdexcept>
#include <vector>

namespace opera {

// The comparator decides what "comes first" means.
//
// The default says a comes before b when a < b, so the smallest value sits at
// the top - a MIN-heap. Pass a different comparator to flip it into a MAX-heap
// without writing a second heap class. The triage queue will do exactly that
// to get "highest severity first".

template <typename T>
struct LessThan {
    bool operator()(const T& a, const T& b) const { return a < b; }
};

template <typename T, typename Compare = LessThan<T>>
class MinHeap {
public:
    explicit MinHeap(Compare comp = Compare()) : before_(comp) {}

    std::size_t size() const { return data_.size(); }
    bool        empty() const { return data_.empty(); }
    void        clear() { data_.clear(); }

    // INSERT
    
    void push(const T& value) {
        data_.push_back(value);
        siftUp(data_.size() - 1);
    }

    // Look at the minimum without removing it.
    const T& top() const {
        if (data_.empty()) {
            throw std::out_of_range("MinHeap::top - heap is empty");
        }
        return data_[0];
    }

    // REMOVE THE MINIMUM
    
    T pop() {
        if (data_.empty()) {
            throw std::out_of_range("MinHeap::pop - heap is empty");
        }
        T result = data_[0];
        data_[0] = data_.back();
        data_.pop_back();
        if (!data_.empty()) {
            siftDown(0);
        }
        return result;
    }

private:
    // Move the element at i upward until its parent comes before it.
    void siftUp(std::size_t i) {
        while (i > 0) {
            const std::size_t parent = (i - 1) / 2;

            if (!before_(data_[i], data_[parent])) {
                break;  // parent already comes first - heap property holds
            }
            swapAt(i, parent);
            i = parent;  // keep climbing from the parent's old position
        }
    }

    // Move the element at i downward until it comes before both children.
    void siftDown(std::size_t i) {
        const std::size_t n = data_.size();

        while (true) {
            const std::size_t left  = 2 * i + 1;
            const std::size_t right = 2 * i + 2;
            std::size_t       best  = i;

            // Find which of the three (node, left child, right child) should be
            // on top. A child may not exist, hence the "< n" checks.
            if (left < n && before_(data_[left], data_[best])) {
                best = left;
            }
            if (right < n && before_(data_[right], data_[best])) {
                best = right;
            }

            if (best == i) {
                break;  // node already comes before both children - done
            }
            swapAt(i, best);
            i = best;  // keep sinking from the child's old position
        }
    }

    void swapAt(std::size_t a, std::size_t b) {
        T tmp    = data_[a];
        data_[a] = data_[b];
        data_[b] = tmp;
    }

    std::vector<T> data_;     // vector storage for the heap, in level-order
    Compare        before_;   // "does a come before b?"
};

}  // namespace opera