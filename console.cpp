#include "console.h"
#include <iostream>
#include <iomanip>

void Console::Output(
    const CompClubConfig& config,
    const std::vector<std::unique_ptr<Event>>& events,
    const std::vector<std::unique_ptr<Table>>& tables
)
{
    std::cout << FormatTime(config.startTime) << std::endl;
    
    for (auto &&event : events)
    {
        std::cout << FormatTime(event->time) << " " << event->id << " ";
        
        if (event->id == 13)
        {
            std::cout << event->error.name;
        }
        else
        {
            std::cout << event->clientName;

            if (event->id == 2 || event->id == 12)
                std::cout << " " << event->tableId;
            
        }
        std::cout << std::endl;
    }
    
    std::cout << FormatTime(config.endTime) << std::endl;

    for (auto &&table : tables)
    {
        std::cout << table->id << " "
            << table->income << " "
            << FormatTime(table->usageTime)
            << std::endl;
    }
}

std::string Console::FormatTime(int time) {
    std::ostringstream oss;
    int hours = time / 60; // Часы
    int mins = time % 60;  // Минуты
    oss << std::setw(2) << std::setfill('0') << hours << ":" 
        << std::setw(2) << std::setfill('0') << mins;
    
    return oss.str();
}