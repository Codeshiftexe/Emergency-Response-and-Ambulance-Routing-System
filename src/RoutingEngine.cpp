// Dijkstra's shortest-path algorithm, using our own MinHeap.

#include "RoutingEngine.hpp"
#include "MinHeap.hpp"
#include <algorithm>
#include <stdexcept>
#include <utility>

namespace opera {

namespace {

// One entry in the heap: "I can reach `node` in `dist` minutes."
struct HeapItem {
    double dist;
    int    node;
};

// Orders heap entries by distance, smallest first. This comparator is what
// turns our general MinHeap into "give me the closest unfinished location".
struct CloserFirst {
    bool operator()(const HeapItem& a, const HeapItem& b) const {
        return a.dist < b.dist;
    }
};

}  // namespace


ShortestPaths::ShortestPaths(std::vector<double> dist, std::vector<int> parent,
                             int source)
    : dist_(std::move(dist)), parent_(std::move(parent)), source_(source) {}

double ShortestPaths::distanceTo(int target) const {
    if (target < 0 || target >= static_cast<int>(dist_.size())) {
        throw std::out_of_range("ShortestPaths::distanceTo - node id out of range");
    }
    return dist_[target];
}

bool ShortestPaths::reachable(int target) const {
    return distanceTo(target) != INF;
}

// Rebuild the route by walking parent links backwards from target to source,
// then reversing. Cost: O(length of the route).
std::vector<int> ShortestPaths::pathTo(int target) const {
    std::vector<int> path;
    if (!reachable(target)) {
        return path;                          // empty means "no route"
    }

    for (int cur = target; cur != -1; cur = parent_[cur]) {
        path.push_back(cur);                  // collected target -> source
    }
    std::reverse(path.begin(), path.end());   // now source -> target
    return path;
}

// dijkstra - algorithm

ShortestPaths dijkstra(const Graph& g, int source) {
    if (!g.hasNode(source)) {
        throw std::out_of_range("dijkstra - source node id out of range");
    }

    // STEP 1: start with "everything is unreachable" 
    // Every location starts at INF with no parent. Only the source is known:
    // it is 0 minutes from itself.
    std::vector<double> dist(g.size(), INF);
    std::vector<int>    parent(g.size(), -1);

    dist[source] = 0.0;

    // The heap holds locations we have reached but not yet finished, ordered
    // by how far away they are. It starts with just the source.
    MinHeap<HeapItem, CloserFirst> frontier;
    frontier.push({0.0, source});

    // STEP 2: repeatedly finish the closest unfinished location
    while (!frontier.empty()) {
        const HeapItem current = frontier.pop();
        const int      u       = current.node;

        // STEP 3: skip stale entries (LAZY DELETION)
        // The same location may be added to the heap multiple times, if a faster
        // route to it is found before it is finished. If we pop a location that
        // has already been finished, ignore it and pop the next one.
        if (current.dist > dist[u]) {
            continue;
        }

        // STEP 4: RELAXATION - look at every road leaving u and see if it improves the route to v
        for (const Edge& road : g.neighbours(u)) {
            const int    v         = road.to;
            const double viaU      = dist[u] + road.weight;

            if (viaU < dist[v]) {
                dist[v]   = viaU;       // found a faster way to v
                parent[v] = u;          // ... and it comes through u
                frontier.push({viaU, v});
            }
        }
    }

    // STEP 5: hand back every distance and every parent
    return ShortestPaths(std::move(dist), std::move(parent), source);
}

}  // namespace opera