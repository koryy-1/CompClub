#include <sstream>
#include "parser.h"
#include "common/types.h"

EventLog* Parser::Parse(std::vector<std::string> content)
{
    CompClubConfig config;
    // todo: validate data
    config.tableCount = stoi(content[0]);
    // todo: parse datetime
    config.startTime = stoi(content[1]);
    config.endTime = stoi(content[1]); // todo: check space
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
        Event* event = new Event();
        event->time = stoi(fields[0]);
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