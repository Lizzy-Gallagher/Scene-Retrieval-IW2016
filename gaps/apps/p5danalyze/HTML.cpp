#include <fstream>
#include "Prepositions.h"

std::string GetFileName(std::string pri_cat) {
    return "html/" + pri_cat + ".html";
}

std::string GetRelativeFileName(std::string ref_cat) {
    return ref_cat + ".html";
}

void PrintPriBlurb(std::ofstream &file, std::string pri_cat, FrequencyStats freq_stats) {
    file << "<h1>" << pri_cat << "</h1>";
    file << "<p>" << "Number of Objects: " << (*freq_stats.cat_count)[pri_cat] << "</p>";
    file << "<p>" << "# Scenes: " << "TODO" << "</p>"; 
}

void PrintTableHeader(std::ofstream &file, const char* prep_names[]) {
    file << "<thead><tr><th>Reference Category</th>";
    for (int i = 0; i < NUM_PREPOSITIONS; i++)
        file << "<th>" << prep_names[i] << "</th>";
    file << "</tr></thead>";
}

void PrintTableRow(std::ofstream &file, std::string pri_cat, std::string ref_cat, PrepositionStats prep_stats, FrequencyStats freq_stats) {
    double div = (double) ((*freq_stats.pair_count)[pri_cat][ref_cat]);
    if (div <= 0.0) return;

    bool all_zero = true;
    double probabilities[NUM_PREPOSITIONS]; 
    for (int i = 0; i < NUM_PREPOSITIONS; i++) {
        probabilities[i] = prep_stats[i] / div; 
        if (prep_stats[i] > 0)
            all_zero = false;
    }
    if (all_zero) return;

    file << "<tr><td><a href=\"" << GetRelativeFileName(ref_cat) << "\">" << ref_cat << "</td>";
    for (int i = 0; i < NUM_PREPOSITIONS; i++)
        file << "<td>" << probabilities[i] << " (" << prep_stats[i] << ") </td>";

    file << "</tr>";
}

void CreatePage(std::string pri_cat, std::map<std::string, PrepositionStats> spec_prep_map,
        FrequencyStats freq_stats, const char* prep_names[]) {
    
    std::ofstream file;
    file.open(GetFileName(pri_cat));
    file << "<!DOCTYPE html><html><head> \
        <link rel=\"stylesheet\" type=\"text/css\" href=\"style.css\"> \
        <script src=\"sorttable.js\"></script></head><body>";

    PrintPriBlurb(file, pri_cat, freq_stats);
    
    file << "<div class=\"datagrid\"><table class=\"sortable\">";
    
    // Table header
    PrintTableHeader(file, prep_names);

    // Table body
    file << "<tbody>";
    for (auto it : spec_prep_map) {
        std::string ref_cat = it.first;
        PrepositionStats prep_stats = it.second;

        PrintTableRow(file, pri_cat, ref_cat, prep_stats, freq_stats);    
    }

    file << "</tbody></table></div>";
    file << "</body></html>";
    file.close();
}

void CreateTOC(PrepMap* prepmap, FrequencyStats freq_stats) {
    PrepMap prep_map = *prepmap;

    std::ofstream file;
    file.open("html/main.html");
     file << "<!DOCTYPE html><html><head> \
        <link rel=\"stylesheet\" type=\"text/css\" href=\"style.css\"> \
        <script src=\"sorttable.js\"></script></head><body>";

    file << "<h1>Table of Contents</h1>";
    file << "<p>Each of object categories were present in the P5D Scenes</p>";
    
    file << "<div class=\"datagrid\"><table class=\"sortable\">"; 
    file << "<thead><tr><th>Primary Object Category</th><th>Number of Appearances</th></tr></thead>"; 
    file << "<tbody>";

    for (auto it : prep_map) {
        std::string cat = it.first;
        file << "<tr><td><a href=\"" << GetRelativeFileName(cat) << "\">" << cat << "</td>" \
            "<td>" << (*freq_stats.cat_count)[cat] << "</td></tr>";
    }
    file << "</tbody></table></div>";
    file << "</body></html>";
    file.close();

}
