#include <iostream>
#include <fstream>
#include "fileReader.h"

std::vector<std::string> FileReader::GetContent(const char* filePath)
{
    std::ifstream file(filePath);
    if (!file.is_open())
        throw std::runtime_error("Ошибка: файл не удалось открыть");
    
    std::string line;
    std::vector<std::string> content;
    
    while (std::getline(file, line))
    {
        content.push_back(line);
    }

    return content;
}