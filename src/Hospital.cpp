// Implementation of Hospital.

#include "Hospital.hpp"
#include <stdexcept>

namespace opera {


    Hospital::Hospital(int id, int nodeId, int totalBeds)
        : id_(id), nodeId_(nodeId), totalBeds_(totalBeds), availableBeds_(totalBeds) {
        if (totalBeds < 0) {
            throw std::invalid_argument("Hospital - totalBeds cannot be negative");
        }
    }

    bool Hospital::reserveBed() {
        if (availableBeds_ <= 0) {
            return false;                 // full: refuse, count unchanged
        }
        --availableBeds_;
        return true;
    }

    bool Hospital::releaseBed() {
        if (availableBeds_ >= totalBeds_) {
            return false;                 // nothing taken: refuse, count unchanged
        }
        ++availableBeds_;
        return true;
    }

} 