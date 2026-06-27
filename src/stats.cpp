#include "stats.h"

#include <algorithm>
#include <numeric>
#include <stdexcept>

Stats computeStats(const std::vector<double>& readings) {
    if (readings.empty()) {
        throw std::invalid_argument("readings must not be empty");
    }

    Stats s;
    s.count = readings.size();
    s.min = *std::min_element(readings.begin(), readings.end());
    s.max = *std::max_element(readings.begin(), readings.end());

    const double sum = std::accumulate(readings.begin(), readings.end(), 0.0);
    s.mean = sum / static_cast<double>(readings.size());

    return s;
}
