// Tests for Hospital and Ambulance.

// Build and run:
//     g++ -std=c++17 -Iinclude tests/test_resources.cpp src/Hospital.cpp src/Ambulance.cpp -o test_resources
//     ./test_resources

#include "Ambulance.hpp"
#include "Hospital.hpp"
#include "test_util.hpp"

#include <stdexcept>
#include <string>

using namespace opera;

static void testHospitalStartsWithAllBedsFree() {
    Hospital h(1, /*nodeId=*/4, /*totalBeds=*/3);
    CHECK_EQ(h.id(), 1);
    CHECK_EQ(h.nodeId(), 4);
    CHECK_EQ(h.totalBeds(), 3);
    CHECK_EQ(h.availableBeds(), 3);
    CHECK(h.hasFreeBed());
}

// Filling the hospital, then trying to over-fill it.
static void testBedsNeverGoNegative() {
    Hospital h(1, 4, 2);

    CHECK(h.reserveBed());              // 2 -> 1
    CHECK(h.reserveBed());              // 1 -> 0
    CHECK_EQ(h.availableBeds(), 0);
    CHECK(!h.hasFreeBed());

    // Full: must refuse AND leave the count alone.
    CHECK(!h.reserveBed());
    CHECK_EQ(h.availableBeds(), 0);

    for (int i = 0; i < 50; ++i) {
        (void)h.reserveBed();
    }
    CHECK_EQ(h.availableBeds(), 0);
}

// Releasing more beds than were ever taken.
static void testBedsNeverExceedCapacity() {
    Hospital h(1, 4, 2);

    CHECK(h.reserveBed());              // 2 -> 1
    CHECK(h.releaseBed());              // 1 -> 2
    CHECK_EQ(h.availableBeds(), 2);

    // Nothing left to release: must refuse and stay at 2.
    CHECK(!h.releaseBed());
    CHECK_EQ(h.availableBeds(), 2);
}

static void testZeroBedHospital() {
    Hospital h(5, 0, 0);
    CHECK(!h.hasFreeBed());
    CHECK(!h.reserveBed());
    CHECK(!h.releaseBed());
    CHECK_EQ(h.availableBeds(), 0);
}

// Negative capacity is impossible data, so the constructor throws.
static void testNegativeCapacityThrows() {
    bool threw = false;
    try {
        Hospital bad(9, 0, -5);
        (void)bad;
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    CHECK(threw);
}

static void testAmbulanceStartsAvailable() {
    Ambulance a(7, /*nodeId=*/2);
    CHECK_EQ(a.id(), 7);
    CHECK_EQ(a.nodeId(), 2);
    CHECK(a.isAvailable());
    CHECK(a.status() == AmbulanceStatus::Available);
    CHECK_EQ(a.assignedEmergency(), -1);
}

// The most important ambulance rule: one emergency at a time.
static void testNoDoubleBooking() {
    Ambulance a(7, 2);

    CHECK(a.assignTo(100));
    CHECK(a.status() == AmbulanceStatus::Busy);
    CHECK_EQ(a.assignedEmergency(), 100);

    // Second assignment must be refused AND must not overwrite job 100.
    CHECK(!a.assignTo(200));
    CHECK_EQ(a.assignedEmergency(), 100);

    CHECK(a.release());
    CHECK(a.isAvailable());
    CHECK_EQ(a.assignedEmergency(), -1);

    // Releasing an idle ambulance is refused.
    CHECK(!a.release());
}

// -1 means "no job", so it must not be accepted as a real emergency id.
static void testNegativeEmergencyIdRefused() {
    Ambulance a(7, 2);
    CHECK(!a.assignTo(-1));
    CHECK(a.isAvailable());
    CHECK_EQ(a.assignedEmergency(), -1);
}

static void testOfflineRules() {
    Ambulance a(7, 2);

    CHECK(a.goOffline());
    CHECK(a.status() == AmbulanceStatus::Offline);
    CHECK(!a.assignTo(100));            // off-duty: cannot dispatch
    CHECK(!a.goOffline());              // already offline
    CHECK(a.goOnline());
    CHECK(a.isAvailable());
    CHECK(!a.goOnline());               // already online

    CHECK(a.assignTo(100));
    CHECK(!a.goOffline());              // mid-call: cannot go offline
    CHECK_EQ(a.assignedEmergency(), 100);
}

static void testMoveToChangesOnlyLocation() {
    Ambulance a(7, 2);
    a.assignTo(100);
    a.moveTo(9);// moveTo changes only location, not status or job
    CHECK_EQ(a.nodeId(), 9);
    CHECK(a.status() == AmbulanceStatus::Busy);
    CHECK_EQ(a.assignedEmergency(), 100);
}

// toString gives readable text for every state.
static void testToString() {
    CHECK(std::string(toString(AmbulanceStatus::Available)) == "Available");
    CHECK(std::string(toString(AmbulanceStatus::Busy)) == "Busy");
    CHECK(std::string(toString(AmbulanceStatus::Offline)) == "Offline");
}

static bool invariantHolds(const Ambulance& a) {
    const bool busy   = (a.status() == AmbulanceStatus::Busy);
    const bool hasJob = (a.assignedEmergency() != -1);
    return busy == hasJob;
}

static void testInvariantAfterEveryOperation() {
    Ambulance a(7, 2);
    CHECK(invariantHolds(a));

    a.assignTo(5);      CHECK(invariantHolds(a));   // allowed
    a.assignTo(6);      CHECK(invariantHolds(a));   // refused
    a.goOffline();      CHECK(invariantHolds(a));   // refused
    a.release();        CHECK(invariantHolds(a));   // allowed
    a.release();        CHECK(invariantHolds(a));   // refused
    a.goOffline();      CHECK(invariantHolds(a));   // allowed
    a.assignTo(7);      CHECK(invariantHolds(a));   // refused
    a.goOnline();       CHECK(invariantHolds(a));   // allowed
    a.assignTo(-1);     CHECK(invariantHolds(a));   // refused
}

int main() {
    testHospitalStartsWithAllBedsFree();
    testBedsNeverGoNegative();
    testBedsNeverExceedCapacity();
    testZeroBedHospital();
    testNegativeCapacityThrows();

    testAmbulanceStartsAvailable();
    testNoDoubleBooking();
    testNegativeEmergencyIdRefused();
    testOfflineRules();
    testMoveToChangesOnlyLocation();
    testToString();
    testInvariantAfterEveryOperation();

    return report("test_resources");
}