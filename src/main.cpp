#include <iostream>
#include <string>
#include <vector>

#include "stats.h"

// Usage:
//   sensor_stats 21.4 22.1 19.8        # numbers as arguments
//   echo "21.4 22.1 19.8" | sensor_stats   # or piped on stdin
int main(int argc, char** argv) {
    std::vector<double> readings;

    if (argc > 1) {
        for (int i = 1; i < argc; ++i) {
            try {
                readings.push_back(std::stod(argv[i]));
            } catch (const std::exception&) {
                std::cerr << "Skipping non-numeric input: " << argv[i] << "\n";
            }
        }
    } else {
        double v;
        while (std::cin >> v) {
            readings.push_back(v);
        }
    }

    if (readings.empty()) {
        std::cerr << "No numeric readings provided.\n";
        return 1;
    }

    const Stats s = computeStats(readings);
    std::cout << "count=" << s.count << " min=" << s.min << " max=" << s.max
              << " mean=" << s.mean << "\n";
    return 0;
}
