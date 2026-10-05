#pragma once

namespace opera {

    enum class AmbulanceStatus {
        Available,   // idle at its location, can be dispatched
        Busy,        // committed to exactly one emergency
        Offline      // out of service
    };

    // Turns a status into text for printing status messages.
    const char* toString(AmbulanceStatus s);

    class Ambulance {
    public:
        Ambulance(int id, int nodeId);

        
        int  id() const { return id_; }
        int  nodeId() const { return nodeId_; }      // current location on the graph
        AmbulanceStatus status() const { return status_; }
        bool isAvailable() const { return status_ == AmbulanceStatus::Available; }

        // Which emergency it is working on, or -1 if none.
        int  assignedEmergency() const { return assignedEmergency_; }

        // Available -> Busy.
        bool assignTo(int emergencyId);

        // Busy -> Available.
        bool release();

        // Available or Busy -> Offline.
        bool goOffline();

        // Offline -> Available.
        bool goOnline();

        void moveTo(int nodeId) { nodeId_ = nodeId; }

    private:
        int id_;
        int nodeId_;
        AmbulanceStatus status_;
        int assignedEmergency_;

    };

}