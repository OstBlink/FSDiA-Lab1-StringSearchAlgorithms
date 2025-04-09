#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>
#include <sstream>
#include <iomanip>
#include "KMP.cpp"
#include "FishAlg.cpp"

// Структура для представления записи из входного файла
struct Record {
    std::string date;
    std::string fullName;
    std::string discipline;
};

// Структура для представления условия поиска
struct SearchCondition {
    int fieldIndex; // 0 для даты, 1 для полного имени, 2 для дисциплины
    std::vector<std::string> patterns;
    int requiredCount;
};

// Функция для чтения записей из входного файла
std::vector<Record> readRecords(const std::string& filename, int n) {
    std::vector<Record> records;
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "Ошибка открытия файла: " << filename << std::endl;
        return records;
    }
    
    std::string line;
    int count = 0;
    
    while (std::getline(file, line) && count < n) {
        std::istringstream iss(line);
        Record record;
        
        // Предполагается формат с разделителем табуляции
        std::getline(iss, record.date, '\t');
        std::getline(iss, record.fullName, '\t');
        std::getline(iss, record.discipline);
        
        records.push_back(record);
        count++;
    }
    
    file.close();
    return records;
}

// Функция для записи результатов в выходной файл
void writeResults(const std::string& filename, 
                 const std::vector<std::pair<int, Record>>& results, 
                 long long searchTime) {
    std::ofstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "Ошибка открытия выходного файла: " << filename << std::endl;
        return;
    }
    
    // Запись заголовка
    file << std::setw(10) << "Строка" << "\t"
         << std::setw(20) << "Дата" << "\t"
         << std::setw(30) << "Полное имя" << "\t"
         << std::setw(30) << "Дисциплина" << std::endl;
    
    // Запись результатов
    for (const auto& result : results) {
        file << std::setw(10) << result.first << "\t"
             << std::setw(20) << result.second.date << "\t"
             << std::setw(30) << result.second.fullName << "\t"
             << std::setw(30) << result.second.discipline << std::endl;
    }
    
    // Запись времени поиска
    file << "Время поиска: " << searchTime << " миллисекунд" << std::endl;
    
    file.close();
}

// Функция для проверки соответствия записи условиям поиска с использованием алгоритма КМП
bool matchesKMP(const Record& record, const std::vector<SearchCondition>& conditions) {
    KMP kmp;
    
    for (const auto& condition : conditions) {
        std::string field;
        
        // Получение соответствующего поля на основе fieldIndex
        switch (condition.fieldIndex) {
            case 0: field = record.date; break;
            case 1: field = record.fullName; break;
            case 2: field = record.discipline; break;
            default: continue;
        }
        
        // Подсчет совпадений для каждого шаблона
        int matchCount = 0;
        for (const auto& pattern : condition.patterns) {
            std::vector<int> matches = kmp.search(field, pattern);
            matchCount += matches.size();
        }
        
        // Если требуемое количество совпадений не достигнуто, возвращаем false
        if (matchCount < condition.requiredCount) {
            return false;
        }
    }
    
    return true;
}

// Функция для проверки соответствия записи условиям поиска с использованием алгоритма Ахо-Корасик
bool matchesAhoCorasick(const Record& record, const std::vector<SearchCondition>& conditions) {
    for (const auto& condition : conditions) {
        std::string field;
        
        // Получение соответствующего поля на основе fieldIndex
        switch (condition.fieldIndex) {
            case 0: field = record.date; break;
            case 1: field = record.fullName; break;
            case 2: field = record.discipline; break;
            default: continue;
        }
        
        AhoCorasick ac;
        
        // Добавление всех шаблонов в автомат Ахо-Корасик
        for (int i = 0; i < condition.patterns.size(); i++) {
            ac.addPattern(condition.patterns[i], i);
        }
        
        // Поиск всех шаблонов в поле
        std::vector<std::pair<int, int>> matches = ac.search(field);
        
        // Подсчет общего количества совпадений
        if (matches.size() < condition.requiredCount) {
            return false;
        }
    }
    
    return true;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Использование: " << argv[0] << " <входной_файл> <n>" << std::endl;
        return 1;
    }
    
    std::string inputFile = argv[1];
    int n = std::stoi(argv[2]);
    
    if (n < 10 || n > 1000000) {
        std::cerr << "n должно быть между 10 и 1000000" << std::endl;
        return 1;
    }
    
    // Чтение записей из входного файла
    std::vector<Record> records = readRecords(inputFile, n);
    
    if (records.empty()) {
        std::cerr << "Не удалось прочитать записи из входного файла" << std::endl;
        return 1;
    }
    
    // Определение условий поиска (обычно это поступает от пользовательского ввода)
    std::vector<SearchCondition> conditions;
    
    // Пример: Поиск записей, где "Programming" встречается хотя бы один раз в поле дисциплины
    SearchCondition condition1;
    condition1.fieldIndex = 2; // дисциплина
    condition1.patterns.push_back("Programming");
    condition1.requiredCount = 1;
    conditions.push_back(condition1);
    
    // Пример: Поиск записей, где "Smith" встречается хотя бы один раз в поле полного имени
    SearchCondition condition2;
    condition2.fieldIndex = 1; // полное имя
    condition2.patterns.push_back("Smith");
    condition2.requiredCount = 1;
    conditions.push_back(condition2);
    
    // Поиск с использованием КМП
    std::vector<std::pair<int, Record>> kmpResults;
    auto kmpStart = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < records.size(); i++) {
        if (matchesKMP(records[i], conditions)) {
            kmpResults.emplace_back(i + 1, records[i]); // +1 для нумерации строк с 1
        }
    }
    
    auto kmpEnd = std::chrono::high_resolution_clock::now();
    auto kmpDuration = std::chrono::duration_cast<std::chrono::milliseconds>(kmpEnd - kmpStart).count();
    
    // Поиск с использованием Ахо-Корасик
    std::vector<std::pair<int, Record>> acResults;
    auto acStart = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < records.size(); i++) {
        if (matchesAhoCorasick(records[i], conditions)) {
            acResults.emplace_back(i + 1, records[i]); // +1 для нумерации строк с 1
        }
    }
    
    auto acEnd = std::chrono::high_resolution_clock::now();
    auto acDuration = std::chrono::duration_cast<std::chrono::milliseconds>(acEnd - acStart).count();
    
    // Запись результатов в выходные файлы
    writeResults("kmp_results.txt", kmpResults, kmpDuration);
    writeResults("ac_results.txt", acResults, acDuration);
    
    std::cout << "Поиск КМП завершен за " << kmpDuration << " мс. Найдено " << kmpResults.size() << " совпадений." << std::endl;
    std::cout << "Поиск Ахо-Корасик завершен за " << acDuration << " мс. Найдено " << acResults.size() << " совпадений." << std::endl;
    
    return 0;
}
