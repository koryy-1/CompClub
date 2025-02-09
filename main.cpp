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
    std::unique_ptr<EventLog> eventLog;
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
    std::unique_ptr<TrackingSystem> trackingSystem = std::make_unique<TrackingSystem>(
        eventLog->config,
        std::move(eventLog->events)
    );

    const std::vector<std::unique_ptr<Event>>& generatedEvents = trackingSystem->GetEvents();
    const std::vector<std::unique_ptr<Table>>& tables = trackingSystem->GetTables();

    // вывод новых событий
    Console::Output(eventLog->config, generatedEvents, tables);
}
