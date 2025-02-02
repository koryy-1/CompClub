#include "console.h"
#include <iostream>

void Console::Output(
    CompClubConfig config,
    std::vector<Event*> events,
    std::vector<Table*> tables
)
{
    std::cout << config.startTime << std::endl;
    
    int count = 0;
    Event* curEvent = events[count];
    while (curEvent != nullptr)
    {
        std::cout << curEvent->time << " " << curEvent->id << " ";
        
        if (curEvent->id == 13)
        {
            std::cout << curEvent->error.name;
        }
        else
        {
            std::cout << curEvent->client->name << " " << curEvent->tableId;
        }
        std::cout << std::endl;
        
        curEvent = events[count++];
    }
    
    std::cout << config.endTime << std::endl;

    // for (size_t i = 0; i < config.tableCount; i++)
    // {
    //     std::cout << tables[i]->id << tables[i]->income 
    //         << tables[i]->usageTime << std::endl;
    // }
}