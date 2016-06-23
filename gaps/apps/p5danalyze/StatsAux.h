// This is the header file for any collection of
// statistics

#ifndef STATS_AUX
#define STATS_AUX

#include <map>
#include <string>

using CountMap = std::map<std::string, int>;
using PairMap = std::map<std::string, std::map<std::string, int>>;

struct FrequencyStats {
    CountMap* cat_count = new CountMap();
    PairMap* pair_count = new PairMap();
};

// PrintFrequencyStats(FrequencyStats stats)

#endif
