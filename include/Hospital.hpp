#pragma once

// A hospital that can receive patients.

namespace opera {

class Hospital {
public:
    Hospital(int id, int nodeId, int totalBeds);

    int id() const { return id_; }
    int nodeId() const { return nodeId_; }          // where it is on the road graph
    int totalBeds() const { return totalBeds_; }
    int availableBeds() const { return availableBeds_; }
    bool hasFreeBed() const { return availableBeds_ > 0; }

    bool reserveBed();

    // Discharge one patient. Returns true if a bed was freed.
    // Returns false - and changes NOTHING - if all beds were already free.
    bool releaseBed();

private:
    int id_;
    int nodeId_;
    int totalBeds_;
    int availableBeds_;
};

}  // namespace opera