#include <fstream>
#include "Prepositions.h"

std::map<std::string, std::ofstream> file_collection;

std::string GetFileName(std::string pri_cat) {
    return "html/" + pri_cat + ".html";
}

std::string GetRelativeFileName(std::string ref_cat) {
    return ref_cat + ".html";
}

void CreatePage(std::string pri_cat, std::map<std::string, PrepositionStats> spec_prep_map,
        FrequencyStats freq_stats, const char* prep_names[]) {
    
    std::ofstream file;
    file.open(GetFileName(pri_cat));
    file << "<!DOCTYPE html><html><head><link rel=\"stylesheet\" type=\"text/css\" href=\"style.css\"></head><body>";

    file << "<h1>" << pri_cat << "</h1>"; 
    
    file << "<ul>";
    for (auto it : spec_prep_map) {
        std::string ref_cat = it.first;
        PrepositionStats prep_stats = it.second;

       double div = (double) ((*freq_stats.pair_count)[pri_cat][ref_cat]);
       if (div <= 0.0) continue;

        bool all_zero = true;
        double probabilities[NUM_PREPOSITIONS]; 
        for (int i = 0; i < NUM_PREPOSITIONS; i++) {
            probabilities[i] = prep_stats[i] / div; 
            if (prep_stats[i] > 0)
                all_zero = false;
        }
        if (all_zero) continue;
            
        file << "<li><a href=\""<< GetRelativeFileName(ref_cat) << "\">" << ref_cat << "</a>";
       
        file << "<div class=\"datagrid\"><table>";
        
        // Table header
        file << "<thead><tr>";
        for (int i = 0; i < NUM_PREPOSITIONS; i++)
            file << "<th>" << prep_names[i] << "</th>";
        file << "</tr></thead>";

        // Table body
        file << "<tbody><tr>";
        for (int i = 0; i < NUM_PREPOSITIONS; i++)
            file << "<td>" << probabilities[i] << " (" << prep_stats[i] << ") </td>";
        file << "</tr></tbody>";
        file << "</table></div>";

        file << "</li>";
    }
    file << "</ul>";

    file << "</body></html>";
    file.close();
}

/*
<div class="datagrid"><table>
<thead><tr><th>header</th><th>header</th><th>header</th><th>header</th></tr></thead>
<tbody><tr><td>data</td><td>data</td><td>data</td><td>data</td></tr>
<tr class="alt"><td>data</td><td>data</td><td>data</td><td>data</td></tr>
<tr><td>data</td><td>data</td><td>data</td><td>data</td></tr>
<tr class="alt"><td>data</td><td>data</td><td>data</td><td>data</td></tr>
<tr><td>data</td><td>data</td><td>data</td><td>data</td></tr>
</tbody>
</table></div>
*/

/*void FinishPage(std::string pri_cat) {
    std::ofstream file;
    file.open(GetFileName(pri_cat));

    file << "</body></html>";
    file.close();
}*/
