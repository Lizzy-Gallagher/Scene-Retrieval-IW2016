#ifndef HTML
#define HTML

#include <string>

void CreatePage(std::string pri_cat, std::map<std::string, PrepositionStats> spec_prep_map, FrequencyStats freq_stats, const char* prep_names[]);

void CreateTOC(PrepMap* prepmap);

#endif
