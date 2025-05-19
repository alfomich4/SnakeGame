#include<fstream>
#include <algorithm>
#include "Record.h"


std::vector<Record> LoadLeaderboard(const std::string& filename)
{
    std::vector<Record> records;
    std::ifstream file(filename);
    if (!file.is_open()) return records;

    Record rec;
    while (file >> rec.name >> rec.score)
    {
        records.push_back(rec);
        if (records.size() >= 10) break; // максимум 10 записей
    }

    return records;
}


void SaveLeaderboard(const std::string& filename, const std::vector<Record>& records)
{
    std::ofstream file(filename);
    if (!file.is_open()) return;

    for (const auto& rec : records)
    {
        file << rec.name << " " << rec.score << "\n";
    }
}

void AddRecord(std::vector<Record>& records, const std::string& name, int score)
{
    records.push_back({ name.substr(0, 3), score }); // обрезаем до 3 символов
    std::sort(records.begin(), records.end(), [](const Record& a, const Record& b)
        {
        return b.score < a.score; // по убыванию
        });

    if (records.size() > 10)
    {
        records.resize(10); // максимум 10
    }
}

