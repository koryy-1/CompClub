#include <iostream>
#include <string>
#include "fileReader.h"
#include "common/types.h"
#include "parser.h"
#include "trackingSystem.h"
#include "console.h"

int main(int argc, char *argv[])
{
    // чтение файла
    std::vector<std::string> content = FileReader::GetContent(argv[1]);

    // парсинг данных
    // todo: использовать unique_ptr
    EventLog* eventLog;
    try
    {
        eventLog = Parser::Parse(content);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        exit(1);
    }

    // генерация новых событий
    TrackingSystem* trackingSystem = new TrackingSystem(eventLog->config, eventLog->events);

    std::vector<Event*> generatedEvents = trackingSystem->GetEvents();
    std::vector<Table*> tables = trackingSystem->GetTables();

    // вывод новых событий
    Console::Output(eventLog->config, generatedEvents, tables);

    delete trackingSystem;
    delete eventLog;
}
