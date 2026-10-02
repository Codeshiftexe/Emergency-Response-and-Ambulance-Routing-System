#pragma once

// A tiny test harness - three macros and a counter.

#include <cmath>
#include <iostream>

// 'inline' lets us define them in a header without the linker complaining
inline int g_checks = 0;
inline int g_fails  = 0;

//multi-line macro to check a condition, and print the file and line number if it fails.
#define CHECK(cond)                                                              \
    do {                                                                         \
        ++g_checks;                                                              \
        if (!(cond)) {                                                           \
            ++g_fails;                                                           \
            std::cerr << "FAIL " << __FILE__ << ":" << __LINE__ << "  " << #cond \
                      << '\n';                                                   \
        }                                                                        \
    } while (0)

//multi-line macro to check equality of two values, and print the file and line number if it fails.
#define CHECK_EQ(actual, expected)                                                 \
    do {                                                                           \
        ++g_checks;                                                                \
        if (!((actual) == (expected))) {                                           \
            ++g_fails;                                                             \
            std::cerr << "FAIL " << __FILE__ << ":" << __LINE__ << "  " << #actual \
                      << " == " << #expected << "  (got " << (actual)              \
                      << ", want " << (expected) << ")\n";                         \
        }                                                                          \
    } while (0)

//multi-line macro to check that two values are approximately equal within a given epsilon, 
//and print the file and line number if it fails.
#define CHECK_NEAR(actual, expected, eps)                                          \
    do {                                                                           \
        ++g_checks;                                                                \
        if (std::fabs((actual) - (expected)) > (eps)) {                            \
            ++g_fails;                                                             \
            std::cerr << "FAIL " << __FILE__ << ":" << __LINE__ << "  " << #actual \
                      << " ~= " << #expected << "  (got " << (actual)              \
                      << ", want " << (expected) << ")\n";                         \
        }                                                                          \
    } while (0)

// Call this as the last line of every test's main():
//     return report("test_heap");
// Returning non-zero marks the test program as failed, which is what lets a
// build script (or GitHub Actions later) notice that something broke.
inline int report(const char* suiteName) {
    std::cout << suiteName << ": " << (g_checks - g_fails) << "/" << g_checks
              << " checks passed\n";
    return g_fails == 0 ? 0 : 1;
}