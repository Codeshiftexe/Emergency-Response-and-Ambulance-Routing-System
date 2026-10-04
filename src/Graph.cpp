// Implementation of the road-network graph declared in Graph.hpp.

#include "Graph.hpp"
#include <stdexcept>

namespace opera {
//addNode - register a new location by name, or return the id of an existing
// location.
int Graph::addNode(const std::string& name) {
    auto it = byName_.find(name);
    if (it != byName_.end()) {
        return it->second;                  // already exists: no duplicate
    }

    const int id = static_cast<int>(adj_.size());
    adj_.push_back({});                     // new node starts with no roads
    labels_.push_back(name);
    byName_[name] = id;
    return id;
}

// addEdge (by id) - add a road.

void Graph::addEdge(int u, int v, double weight, bool bidirectional) {
    // Validate the ids and weight before modifying the graph.
    if (!hasNode(u) || !hasNode(v)) {
        throw std::out_of_range("Graph::addEdge - node id out of range");
    }
    // Validate the weight before modifying the graph.
    if (weight < 0.0) {
        throw std::invalid_argument("Graph::addEdge - negative weight not allowed");
    }
    // Add the road(s) to the adjacency list. O(1) each.
    adj_[u].push_back({v, weight});         // road u -> v
    if (bidirectional) {
        adj_[v].push_back({u, weight});     // and v -> u for a two-way road
    }
}

// addEdge (by name) - convenience wrapper for the data-file loader.

void Graph::addEdge(const std::string& u, const std::string& v, double weight,
                    bool bidirectional) {
    const int uid = addNode(u);
    const int vid = addNode(v);
    addEdge(uid, vid, weight, bidirectional);
}

// nodeId - return the id of a location by name, or -1 if it is unknown. O(1)
int Graph::nodeId(const std::string& name) const {
    auto it = byName_.find(name);
    return it == byName_.end() ? -1 : it->second;
}

const std::string& Graph::nodeName(int id) const {
    if (!hasNode(id)) {
        throw std::out_of_range("Graph::nodeName - node id out of range");
    }
    return labels_[id];
}

// neighbours - all roads leaving u. This is what Dijkstra's inner loop calls.
// Cost: O(1) - it returns a reference to the existing list, no copying.

const std::vector<Edge>& Graph::neighbours(int u) const {
    if (!hasNode(u)) {
        throw std::out_of_range("Graph::neighbours - node id out of range");
    }
    return adj_[u];
}

int Graph::edgeCount() const {
    int total = 0;
    for (const auto& roads : adj_) {
        total += static_cast<int>(roads.size());
    }
    return total;
}

}  // namespace opera