#include <sstream>
#include <iostream>
#include "parser.h"
#include "common/types.h"

EventLog* Parser::Parse(std::vector<std::string> content)
{
    CompClubConfig config;

    if (content.empty()) {
        throw std::runtime_error("Ошибка: файл пуст");
    }

    config.tableCount = stoi(content[0]);

    std::istringstream iss(content[1]);
    std::string startTime, endTime;
    iss >> startTime;
    iss >> endTime;
    config.startTime = ParseTime(startTime);
    config.endTime = ParseTime(endTime);

    config.hourlyCost = stoi(content[2]);

    std::vector<Event*> events;
    for (size_t i = 3; i < content.size(); i++)
    {
        std::istringstream iss(content[i]);
        std::vector<std::string> fields;
        std::string subs;
        while (std::getline( iss, subs, ' ' ) ) {
            fields.push_back(subs);
        }

        if (fields.size() < 3) {
            throw std::runtime_error("Ошибка в строке " + std::to_string(i + 1) + ": " + content[i]);
        }

        Event* event = new Event();
        event->time = ParseTime(fields[0]);
        event->id = stoi(fields[1]);

        Client* client = new Client();
        client->name = fields[2];
        event->client = client;
        if (event->id == 2)
            event->tableId = stoi(fields[3]);
        
        events.push_back(event);
    }

    EventLog* eventLog = new EventLog();

    eventLog->config = config;
    eventLog->events = events;

    return eventLog;
}

int Parser::ParseTime(const std::string& timeStr) {
    std::istringstream iss(timeStr);
    int hours, minutes;
    char colon;
    iss >> hours >> colon >> minutes;
    return hours * 60 + minutes * 1;
}