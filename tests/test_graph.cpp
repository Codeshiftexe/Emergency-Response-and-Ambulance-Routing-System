// Tests for Graph.
// Build and run:
//     g++ -std=c++17 -Iinclude tests/test_graph.cpp src/Graph.cpp -o test_graph./test_graph

#include "Graph.hpp"
#include "test_util.hpp"
#include <stdexcept>

using namespace opera;


static void testNodesGetSequentialIds() {
    Graph g;
    CHECK_EQ(g.size(), 0);

    const int hospital = g.addNode("Civil_Hospital");
    const int station  = g.addNode("Rail_Station");

    CHECK_EQ(hospital, 0);
    CHECK_EQ(station, 1);
    CHECK_EQ(g.size(), 2);

    CHECK_EQ(g.nodeId("Rail_Station"), station);        // name -> id
    CHECK(g.nodeName(hospital) == "Civil_Hospital");    // id -> name
}

static void testAddNodeIsIdempotent() {
    Graph g;
    const int first  = g.addNode("Market_Chowk");
    const int second = g.addNode("Market_Chowk");

    CHECK_EQ(first, second);
    CHECK_EQ(g.size(), 1);
}

static void testUnknownNameLookupChangesNothing() {
    Graph g;
    g.addNode("Central_Square");

    CHECK_EQ(g.nodeId("Nowhere"), -1);
    CHECK_EQ(g.nodeId("Nowhere"), -1);   // twice, in case the first call inserted it
    CHECK_EQ(g.size(), 1);
}

static void testTwoWayRoad() {
    Graph g;
    g.addEdge("Central_Square", "Rail_Station", 4.0);   // two-way by default

    const int a = g.nodeId("Central_Square");
    const int b = g.nodeId("Rail_Station");

    CHECK_EQ(g.edgeCount(), 2);

    CHECK_EQ(g.neighbours(a).size(), 1u);
    CHECK_EQ(g.neighbours(a)[0].to, b);
    CHECK_NEAR(g.neighbours(a)[0].weight, 4.0, 1e-9);

    CHECK_EQ(g.neighbours(b).size(), 1u);
    CHECK_EQ(g.neighbours(b)[0].to, a);                 // the reverse direction
}


static void testOneWayRoad() {
    Graph g;
    g.addEdge("Riverside", "Trauma_Centre", 9.0, /*bidirectional=*/false);

    const int from = g.nodeId("Riverside");
    const int to   = g.nodeId("Trauma_Centre");

    CHECK_EQ(g.edgeCount(), 1);
    CHECK_EQ(g.neighbours(from).size(), 1u);
    CHECK_EQ(g.neighbours(to).size(), 0u);              // no road back
}

static void testAddEdgeByNameCreatesNodes() {
    Graph g;
    g.addEdge("North_Gate", "Hilltop_Colony", 5.0);

    CHECK_EQ(g.size(), 2);
    CHECK(g.nodeId("North_Gate") != -1);
    CHECK(g.nodeId("Hilltop_Colony") != -1);
}

static void testBadInputIsRejectedCleanly() {
    Graph g;
    g.addNode("A");
    g.addNode("B");

    // Negative travel time: Dijkstra would give wrong answers, so refuse it.
    bool threwNegative = false;
    try {
        g.addEdge(0, 1, -3.0);
    } catch (const std::invalid_argument&) {
        threwNegative = true;
    }
    CHECK(threwNegative);

    // Road to a node that does not exist.
    bool threwBadId = false;
    try {
        g.addEdge(0, 99, 1.0);
    } catch (const std::out_of_range&) {
        threwBadId = true;
    }
    CHECK(threwBadId);

    // Name of a node that does not exist.
    bool threwBadName = false;
    try {
        (void)g.nodeName(99);
    } catch (const std::out_of_range&) {
        threwBadName = true;
    }
    CHECK(threwBadName);

    // After three failed operations, nothing should have changed.
    CHECK_EQ(g.size(), 2);
    CHECK_EQ(g.edgeCount(), 0);
}

static void testZeroWeightIsAllowed() {
    Graph g;
    bool threw = false;
    try {
        g.addEdge("Gate_1", "Gate_2", 0.0);
    } catch (...) {
        threw = true;
    }
    CHECK(!threw);
    CHECK_EQ(g.edgeCount(), 2);
}

int main() {
    testNodesGetSequentialIds();
    testAddNodeIsIdempotent();
    testUnknownNameLookupChangesNothing();
    testTwoWayRoad();
    testOneWayRoad();
    testAddEdgeByNameCreatesNodes();
    testBadInputIsRejectedCleanly();
    testZeroWeightIsAllowed();

    return report("test_graph");
}