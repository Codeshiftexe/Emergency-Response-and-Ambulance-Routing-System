#pragma once
// The road network, stored as a weighted graph using an ADJACENCY LIST.
#include <string>
#include <unordered_map>
#include <vector>
namespace opera {

struct Edge {
    int    to;      // id of the node this road leads to
    double weight;  // travel time in minutes, never negative
};

class Graph {
public:
    // Register a location by name and get back its id.
    // IDs are handed out in order: the first node is 0, the next is 1, and so
    // on.
    int addNode(const std::string& name);

    // Add a road between two node ids.
    // bidirectional = true   two-way road: adds u->v AND v->u
    // bidirectional = false  one-way road: adds only u->v
    //
    // Throws std::out_of_range if either id does not exist.
    // Throws std::invalid_argument if weight is negative - because Dijkstra
    // gives WRONG answers on negative weights
    void addEdge(int u, int v, double weight, bool bidirectional = true);

    // Same as above but by name. Creates either node if it is new.
    void addEdge(const std::string& u, const std::string& v, double weight,
                 bool bidirectional = true);

    // Name -> id. Throws std::out_of_range for an unknown name.
    int nodeId(const std::string& name) const;

    // Id -> name. Throws std::out_of_range for an invalid id.
    const std::string& nodeName(int id) const;

    int  size() const { return static_cast<int>(adj_.size()); }
    bool hasNode(int id) const { return id >= 0 && id < size(); }

    // All roads leaving node u. Returned by CONST reference: callers can read
    // the list without copying it, but cannot modify it behind Graph's back.
    const std::vector<Edge>& neighbours(int u) const;

    // Number of stored directed edges. A two-way road counts as 2.
    int edgeCount() const;

private:
    // adj_[u] is the list of roads leaving node u.
    // Node ids are 0..size()-1, so a node's id is its index here - looking up
    // a node's roads is a single array access, O(1).
    std::vector<std::vector<Edge>> adj_;

    // labels_[id] is the human-readable name of node id.
    std::vector<std::string> labels_;

    // TODO: replace with our own HashMap once it is written. Kept as
    // std::unordered_map for now so the graph works before the hash table does.
    std::unordered_map<std::string, int> byName_;

    // INVARIANT: adj_, labels_ and byName_ always describe the same set of
    // nodes - same count, same ids. That is why all three are private and only
    // addNode() ever adds to them.
};

}  // namespace opera