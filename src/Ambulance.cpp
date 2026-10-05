// Implementation of Ambulance.

#include "Ambulance.hpp"
namespace opera {

const char* toString(AmbulanceStatus s) {
    switch (s) {
        case AmbulanceStatus::Available: return "Available";
        case AmbulanceStatus::Busy:      return "Busy";
        case AmbulanceStatus::Offline:   return "Offline";
    }
    return "Unknown";
}

Ambulance::Ambulance(int id, int nodeId)
    : id_(id),
      nodeId_(nodeId),
      status_(AmbulanceStatus::Available),
      assignedEmergency_(-1) {}

// assignTo:  Available -> Busy
bool Ambulance::assignTo(int emergencyId) {
    if (emergencyId < 0) {
        return false;                          // -1 means "no job"; not a real id
    }
    if (status_ != AmbulanceStatus::Available) {
        return false;                          // already Busy, or Offline
    }
    status_            = AmbulanceStatus::Busy;   // change both together
    assignedEmergency_ = emergencyId;
    return true;
}

// release:  Busy -> Available
bool Ambulance::release() {
    if (status_ != AmbulanceStatus::Busy) {
        return false;                          // nothing to release
    }
    status_            = AmbulanceStatus::Available;   // change both together
    assignedEmergency_ = -1;
    return true;
}
// goOffline:  Available -> Offline
bool Ambulance::goOffline() {
    if (status_ != AmbulanceStatus::Available) {
        return false;
    }
    status_ = AmbulanceStatus::Offline;        // job stays -1 
    return true;
}

// goOnline:  Offline -> Available
bool Ambulance::goOnline() {
    if (status_ != AmbulanceStatus::Offline) {
        return false;
    }
    status_ = AmbulanceStatus::Available;
    return true;
}

}