#pragma once
#include <string>
#include <vector>
struct Record 
{
    std::string name; // максимум 3 символа
    int score=0;
};
std::vector<Record> LoadLeaderboard(const std::string& filename);
void SaveLeaderboard(const std::string& filename, const std::vector<Record>& records);
void AddRecord(std::vector<Record>& records, const std::string& name, int score);
