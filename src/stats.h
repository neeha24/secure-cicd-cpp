#ifndef STATS_H
#define STATS_H

#include <cstddef>
#include <vector>

// A small "sensor reading processor" — the kind of utility an embedded /
// hardware-integrated product team might use. Deliberately tiny so the focus
// stays on the PIPELINE around it, not the app itself.
struct Stats {
    double min;
    double max;
    double mean;
    std::size_t count;
};

// Computes min, max and mean over a set of readings.
// Throws std::invalid_argument if the input is empty.
Stats computeStats(const std::vector<double>& readings);

#endif  // STATS_H
