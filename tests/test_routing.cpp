// Tests for Dijkstra (RoutingEngine).
// Build and run:
//     g++ -std=c++17 -Iinclude tests/test_routing.cpp src/Graph.cpp src/RoutingEngine.cpp -o test_routing
//     ./test_routing

#include "Graph.hpp"
#include "RoutingEngine.hpp"
#include "test_util.hpp"

#include <stdexcept>
#include <vector>

using namespace opera;

// The test graph is a small directed graph with one unreachable node. The nodes are numbered 0..5, and the edges are:
//   0 -> 1 (4.0)
//   0 -> 2 (1.0)
//   2 -> 1 (2.0)
//   1 -> 3 (5.0)
//   2 -> 3 (8.0)
//   3 -> 4 (3.0)   

static Graph makeTestGraph() {
    Graph g;
    for (const char* name : {"N0", "N1", "N2", "N3", "N4", "Island"}) {
        g.addNode(name);
    }
    // The final 'false' means one-way.
    g.addEdge(0, 1, 4.0, false);
    g.addEdge(0, 2, 1.0, false);
    g.addEdge(2, 1, 2.0, false);
    g.addEdge(1, 3, 5.0, false);
    g.addEdge(2, 3, 8.0, false);
    g.addEdge(3, 4, 3.0, false);
    return g;
}

// Adds up the road weights along a path. Used to check that the route
// pathTo() returns really does cost what distanceTo() claims.
static double walkPath(const Graph& g, const std::vector<int>& path, bool& valid) {
    valid = true;
    double total = 0.0;
    for (std::size_t i = 0; i + 1 < path.size(); ++i) {
        bool found = false;
        for (const Edge& e : g.neighbours(path[i])) {
            if (e.to == path[i + 1]) {
                total += e.weight;
                found = true;
                break;
            }
        }
        if (!found) valid = false;   // path uses a road that does not exist
    }
    return total;
}

// Every distance matches the hand-worked answer.

static void testDistances() {
    const Graph g = makeTestGraph();
    const ShortestPaths sp = dijkstra(g, 0);

    CHECK_NEAR(sp.distanceTo(0), 0.0, 1e-9);
    CHECK_NEAR(sp.distanceTo(2), 1.0, 1e-9);
    CHECK_NEAR(sp.distanceTo(1), 3.0, 1e-9);    // relaxation improved 4 -> 3
    CHECK_NEAR(sp.distanceTo(3), 8.0, 1e-9);    // relaxation improved 9 -> 8
    CHECK_NEAR(sp.distanceTo(4), 11.0, 1e-9);
}

// The reconstructed route is the right sequence of locations.

static void testPathIsCorrect() {
    const Graph g = makeTestGraph();
    const ShortestPaths sp = dijkstra(g, 0);

    const std::vector<int> expected = {0, 2, 1, 3, 4};
    const std::vector<int> actual   = sp.pathTo(4);

    CHECK_EQ(actual.size(), expected.size());
    CHECK(actual == expected);
}

// Distance and route must AGREE: walking the returned route and adding up its
// roads must give exactly the reported distance.
//
// This catches a bug the two tests above can miss. If the code updated dist[]
// correctly but forgot to update parent[], every distance would be right while
// the route itself was wrong. This test checks the two arrays against each
// other, for every reachable location.

static void testPathAgreesWithDistance() {
    const Graph g = makeTestGraph();
    const ShortestPaths sp = dijkstra(g, 0);

    for (int target = 0; target < g.size(); ++target) {
        if (!sp.reachable(target)) continue;

        const std::vector<int> path = sp.pathTo(target);
        CHECK(!path.empty());
        CHECK_EQ(path.front(), 0);          // every route starts at the source
        CHECK_EQ(path.back(), target);      // and ends at the target

        bool valid = false;
        const double walked = walkPath(g, path, valid);
        CHECK(valid);                       // every step is a real road
        CHECK_NEAR(walked, sp.distanceTo(target), 1e-9);
    }
}


// An unreachable location reports INF and an empty route - not garbage,
// not a crash.

static void testUnreachable() {
    const Graph g = makeTestGraph();
    const ShortestPaths sp = dijkstra(g, 0);

    CHECK(!sp.reachable(5));
    CHECK(sp.distanceTo(5) == INF);
    CHECK(sp.pathTo(5).empty());
}

// One-way roads are respected. From node 4 you cannot get anywhere, because
// every road points INTO it, not out.

static void testOneWayRoadsRespected() {
    const Graph g = makeTestGraph();
    const ShortestPaths sp = dijkstra(g, 4);

    CHECK_NEAR(sp.distanceTo(4), 0.0, 1e-9);
    CHECK(!sp.reachable(0));
    CHECK(!sp.reachable(3));
}

// On two-way roads, A to B costs the same as B to A.

static void testTwoWayRoadsAreSymmetric() {
    Graph g;
    g.addEdge("Station", "Market", 2.0);
    g.addEdge("Market", "Hospital", 3.0);

    const int station  = g.nodeId("Station");
    const int hospital = g.nodeId("Hospital");

    CHECK_NEAR(dijkstra(g, station).distanceTo(hospital), 5.0, 1e-9);
    CHECK_NEAR(dijkstra(g, hospital).distanceTo(station), 5.0, 1e-9);
}

// Small and strange graphs must not break anything.

static void testEdgeCases() {
    // One location, no roads.
    Graph single;
    single.addNode("Only");
    const ShortestPaths sp1 = dijkstra(single, 0);
    CHECK_NEAR(sp1.distanceTo(0), 0.0, 1e-9);
    CHECK_EQ(sp1.pathTo(0).size(), 1u);        // path to yourself is just you

    // A road from a location back to itself must not cause an infinite loop.
    Graph loop;
    loop.addEdge("X", "X", 7.0, false);
    CHECK_NEAR(dijkstra(loop, 0).distanceTo(0), 0.0, 1e-9);

    // Asking for a source that does not exist must throw.
    bool threw = false;
    try {
        (void)dijkstra(single, 42);
    } catch (const std::out_of_range&) {
        threw = true;
    }
    CHECK(threw);
}

int main() {
    testDistances();
    testPathIsCorrect();
    testPathAgreesWithDistance();
    testUnreachable();
    testOneWayRoadsRespected();
    testTwoWayRoadsAreSymmetric();
    testEdgeCases();

    return report("test_routing");
}