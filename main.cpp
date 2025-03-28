#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>
#include <sstream>
#include <iomanip>
#include "KMP.cpp"
#include "FishAlg.cpp"

// Structure to represent a record from the input file
struct Record {
    std::string date;
    std::string fullName;
    std::string discipline;
};

// Structure to represent a search condition
struct SearchCondition {
    int fieldIndex; // 0 for date, 1 for fullName, 2 for discipline
    std::vector<std::string> patterns;
    int requiredCount;
};

// Function to read records from the input file
std::vector<Record> readRecords(const std::string& filename, int n) {
    std::vector<Record> records;
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return records;
    }
    
    std::string line;
    int count = 0;
    
    while (std::getline(file, line) && count < n) {
        std::istringstream iss(line);
        Record record;
        
        // Assuming tab-delimited format
        std::getline(iss, record.date, '\t');
        std::getline(iss, record.fullName, '\t');
        std::getline(iss, record.discipline);
        
        records.push_back(record);
        count++;
    }
    
    file.close();
    return records;
}

// Function to write results to an output file
void writeResults(const std::string& filename, 
                 const std::vector<std::pair<int, Record>>& results, 
                 long long searchTime) {
    std::ofstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "Error opening output file: " << filename << std::endl;
        return;
    }
    
    // Write header
    file << std::setw(10) << "Line" << "\t"
         << std::setw(20) << "Date" << "\t"
         << std::setw(30) << "Full Name" << "\t"
         << std::setw(30) << "Discipline" << std::endl;
    
    // Write results
    for (const auto& result : results) {
        file << std::setw(10) << result.first << "\t"
             << std::setw(20) << result.second.date << "\t"
             << std::setw(30) << result.second.fullName << "\t"
             << std::setw(30) << result.second.discipline << std::endl;
    }
    
    // Write search time
    file << "Search time: " << searchTime << " milliseconds" << std::endl;
    
    file.close();
}

// Function to check if a record matches the search conditions using KMP
bool matchesKMP(const Record& record, const std::vector<SearchCondition>& conditions) {
    KMP kmp;
    
    for (const auto& condition : conditions) {
        std::string field;
        
        // Get the appropriate field based on fieldIndex
        switch (condition.fieldIndex) {
            case 0: field = record.date; break;
            case 1: field = record.fullName; break;
            case 2: field = record.discipline; break;
            default: continue;
        }
        
        // Count matches for each pattern
        int matchCount = 0;
        for (const auto& pattern : condition.patterns) {
            std::vector<int> matches = kmp.search(field, pattern);
            matchCount += matches.size();
        }
        
        // If the required count is not met, return false
        if (matchCount < condition.requiredCount) {
            return false;
        }
    }
    
    return true;
}

// Function to check if a record matches the search conditions using Aho-Corasick
bool matchesAhoCorasick(const Record& record, const std::vector<SearchCondition>& conditions) {
    for (const auto& condition : conditions) {
        std::string field;
        
        // Get the appropriate field based on fieldIndex
        switch (condition.fieldIndex) {
            case 0: field = record.date; break;
            case 1: field = record.fullName; break;
            case 2: field = record.discipline; break;
            default: continue;
        }
        
        AhoCorasick ac;
        
        // Add all patterns to the Aho-Corasick automaton
        for (int i = 0; i < condition.patterns.size(); i++) {
            ac.addPattern(condition.patterns[i], i);
        }
        
        // Search for all patterns in the field
        std::vector<std::pair<int, int>> matches = ac.search(field);
        
        // Count total matches
        if (matches.size() < condition.requiredCount) {
            return false;
        }
    }
    
    return true;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <input_file> <n>" << std::endl;
        return 1;
    }
    
    std::string inputFile = argv[1];
    int n = std::stoi(argv[2]);
    
    if (n < 10 || n > 1000000) {
        std::cerr << "n must be between 10 and 1000000" << std::endl;
        return 1;
    }
    
    // Read records from the input file
    std::vector<Record> records = readRecords(inputFile, n);
    
    if (records.empty()) {
        std::cerr << "No records read from the input file" << std::endl;
        return 1;
    }
    
    // Define search conditions (this would normally come from user input)
    std::vector<SearchCondition> conditions;
    
    // Example: Search for records where "Programming" appears at least once in the discipline field
    SearchCondition condition1;
    condition1.fieldIndex = 2; // discipline
    condition1.patterns.push_back("Programming");
    condition1.requiredCount = 1;
    conditions.push_back(condition1);
    
    // Example: Search for records where "Smith" appears at least once in the fullName field
    SearchCondition condition2;
    condition2.fieldIndex = 1; // fullName
    condition2.patterns.push_back("Smith");
    condition2.requiredCount = 1;
    conditions.push_back(condition2);
    
    // Search using KMP
    std::vector<std::pair<int, Record>> kmpResults;
    auto kmpStart = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < records.size(); i++) {
        if (matchesKMP(records[i], conditions)) {
            kmpResults.emplace_back(i + 1, records[i]); // +1 for 1-based line numbering
        }
    }
    
    auto kmpEnd = std::chrono::high_resolution_clock::now();
    auto kmpDuration = std::chrono::duration_cast<std::chrono::milliseconds>(kmpEnd - kmpStart).count();
    
    // Search using Aho-Corasick
    std::vector<std::pair<int, Record>> acResults;
    auto acStart = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < records.size(); i++) {
        if (matchesAhoCorasick(records[i], conditions)) {
            acResults.emplace_back(i + 1, records[i]); // +1 for 1-based line numbering
        }
    }
    
    auto acEnd = std::chrono::high_resolution_clock::now();
    auto acDuration = std::chrono::duration_cast<std::chrono::milliseconds>(acEnd - acStart).count();
    
    // Write results to output files
    writeResults("kmp_results.txt", kmpResults, kmpDuration);
    writeResults("ac_results.txt", acResults, acDuration);
    
    std::cout << "KMP search completed in " << kmpDuration << " ms. Found " << kmpResults.size() << " matches." << std::endl;
    std::cout << "Aho-Corasick search completed in " << acDuration << " ms. Found " << acResults.size() << " matches." << std::endl;
    
    return 0;
}
