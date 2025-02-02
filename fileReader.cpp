#include <iostream>
#include <fstream>
#include "fileReader.h"

std::vector<std::string> FileReader::GetContent(char* filePath)
{
    std::ifstream file(filePath);
    if (!file.is_open())
    {
        std::cout << "file is not open" << std::endl;
        exit(1);
    }
    
    std::string line;
    std::vector<std::string> content;
    
    while (std::getline(file, line))
    {
        content.push_back(line);
    }

    return content;
}