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
    std::unique_ptr<Parser> parser = std::make_unique<Parser>(content);

    CompClubConfig config = parser->GetConfig();
    std::vector<std::unique_ptr<Event>> events = parser->GetEvents();

    // генерация новых событий
    std::unique_ptr<TrackingSystem> trackingSystem = std::make_unique<TrackingSystem>(
        config,
        std::move(events)
    );

    const std::vector<std::unique_ptr<Event>>& generatedEvents = trackingSystem->GetEvents();
    const std::vector<std::unique_ptr<Table>>& tables = trackingSystem->GetTables();

    // вывод новых событий
    Console::Output(config, generatedEvents, tables);
}
