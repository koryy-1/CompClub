#include <iostream>
#include <fstream>
#include <string>
#include "fileReader.h"
#include "common/types.h"
#include "parser.h"
#include "trackingSystem.h"
#include "console.h"

int main(int argc, char *argv[])
{
    // read file
    std::cout << argv[1] << std::endl;

    std::vector<std::string> content = FileReader::GetContent(argv[1]);

    // parse file to another struct
    EventLog* eventLog;
    try
    {
        eventLog = Parser::Parse(content);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    // generate events and create new struct with new events
    TrackingSystem* trackingSystem = new TrackingSystem(eventLog->config, eventLog->events);

    std::vector<Event*> generatedEvents = trackingSystem->GetEvents();
    std::vector<Table*> tables = trackingSystem->GetTables();

    // output new struct
    Console::Output(eventLog->config, generatedEvents, tables);
}
