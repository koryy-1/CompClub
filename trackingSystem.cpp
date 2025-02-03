#include <algorithm>
#include <cmath>
#include "trackingSystem.h"
#include "utils.h"

TrackingSystem::TrackingSystem(CompClubConfig config, std::vector<Event*> events)
    : m_config(config), m_events(events)
{
    for (size_t i = 1; i <= config.tableCount; i++)
    {
        Table* table = new Table();
        table->id = i;
        m_tables.push_back(table);
    }
    
    Handle();

    CalculateIncome();
}

TrackingSystem::~TrackingSystem()
{
    for (size_t i = 0; i < m_generatedEvents.size(); i++)
    {
        delete m_generatedEvents[i];
    }
    for (size_t i = 0; i < m_tables.size(); i++)
    {
        delete m_tables[i];
    }
    for (size_t i = 0; i < m_clientList.size(); i++)
    {
        delete m_clientList[i];
    }
}

void TrackingSystem::Handle()
{
    for (size_t i = 0; i < m_events.size(); i++)
    {
        // сначала идет набор клиентов
        m_generatedEvents.push_back(m_events[i]);
        switch (m_events[i]->id)
        {
        case 1:
        {
            if (m_config.startTime > m_events[i]->time)
            {
                Event* event = CreateErrorEvent(m_events[i]->time, "NotOpenYet");
                m_generatedEvents.push_back(event);
            }
            else if (Utils::FindClientIndexByName(m_clientList, m_events[i]->client->name) != -1)
            {
                Event* event = CreateErrorEvent(m_events[i]->time, "YouShallNotPass");
                m_generatedEvents.push_back(event);
            }
            else
            {
                m_events[i]->client->isInsideClub = true;
                m_clientList.push_back(m_events[i]->client);
            }
            break;
        }
        case 2:
        {
            int clientIndex = Utils::FindClientIndexByName(m_clientList, m_events[i]->client->name);
            if (clientIndex == -1)
            {
                Event* event = CreateErrorEvent(m_events[i]->time, "ClientUnknown");
                m_generatedEvents.push_back(event);
                break;
            }

            if (m_tables[m_events[i]->tableId - 1]->isBusy)
            {
                Event* event = CreateErrorEvent(m_events[i]->time, "PlaceIsBusy");
                m_generatedEvents.push_back(event);
                break;
            }
            else
            {
                // если клиент сменяет стол, то помечаем предыдущий как не занятый
                if (m_clientList[clientIndex]->occupiedTable)
                {
                    m_tables[m_clientList[clientIndex]->tableUsageSessions.back()->tableId - 1]->isBusy = false;
                    m_tables[m_events[i]->tableId - 1]->isBusy = true;
                    m_clientList[clientIndex]->tableUsageSessions.back()->endTime = m_events[i]->time;

                    TableUsageSession* session = new TableUsageSession();
                    session->startTime = m_events[i]->time;
                    session->tableId = m_events[i]->tableId;
                    m_clientList[clientIndex]->tableUsageSessions.push_back(session);
                }
                else
                {
                    m_tables[m_events[i]->tableId - 1]->isBusy = true;

                    // тут придется каждый стол учитывать
                    TableUsageSession* session = new TableUsageSession();
                    session->startTime = m_events[i]->time;
                    session->tableId = m_events[i]->tableId;
                    m_clientList[clientIndex]->tableUsageSessions.push_back(session);
                    m_clientList[clientIndex]->occupiedTable = true;
                }
            }
            break;
        }
        // потом идет набор очереди
        case 3:
        {
            int clientIndex = Utils::FindClientIndexByName(m_clientList, m_events[i]->client->name);
            if (clientIndex == -1)
            {
                Event* event = CreateErrorEvent(m_events[i]->time, "ClientUnknown");
                m_generatedEvents.push_back(event);
                break;
            }

            if (Utils::FindTableIndex(m_tables, false) != -1)
            {
                Event* event = CreateErrorEvent(m_events[i]->time, "ICanWaitNoLonger!");
                m_generatedEvents.push_back(event);
                break;
            }
            if (m_queue.size() == m_config.tableCount)
            {
                Event* event = CreateEvent(m_events[i]->time, 11, m_events[i]->client->name, 0);
                m_generatedEvents.push_back(event);
                m_clientList[clientIndex]->isInsideClub = false;
                break;
            }
            else
            {
                m_queue.push(m_clientList[clientIndex]);
            }
            break;
        }
        case 4:
        {
            int clientIndex = Utils::FindClientIndexByName(m_clientList, m_events[i]->client->name);
            if (clientIndex == -1)
            {
                Event* event = CreateErrorEvent(m_events[i]->time, "ClientUnknown");
                m_generatedEvents.push_back(event);
                break;
            }
            else
            {
                m_clientList[clientIndex]->tableUsageSessions.back()->endTime = m_events[i]->time;
                m_clientList[clientIndex]->occupiedTable = false;
                m_clientList[clientIndex]->isInsideClub = false;

                if (m_queue.empty())
                {
                    m_tables[m_clientList[clientIndex]->tableUsageSessions.back()->tableId - 1]->isBusy = false;
                }
                else
                {
                    Client* client = m_queue.front();
                    int clientFromQueueIndex = Utils::FindClientIndexByName(m_clientList, client->name);
                    m_queue.pop();

                    TableUsageSession* session = new TableUsageSession();
                    session->startTime = m_events[i]->time;
                    session->tableId = m_clientList[clientIndex]->tableUsageSessions.back()->tableId;
                    m_clientList[clientFromQueueIndex]->tableUsageSessions.push_back(session);

                    m_clientList[clientFromQueueIndex]->tableUsageSessions.back()->startTime = m_events[i]->time;
                    m_clientList[clientFromQueueIndex]->tableUsageSessions.back()->tableId = m_clientList[clientIndex]->tableUsageSessions.back()->tableId;
                    m_clientList[clientFromQueueIndex]->occupiedTable = true;

                    Event* event = CreateEvent(
                        m_events[i]->time,
                        12,
                        m_clientList[clientFromQueueIndex]->name,
                        m_clientList[clientFromQueueIndex]->tableUsageSessions.back()->tableId
                    );
                    m_generatedEvents.push_back(event);
                }
            }
            break;
        }
        default:
            break;
        }
    }

    // выгнать всех оставшихся клиентов через 11 событие, при этом отсортировать клиентов в алф порядке
    std::vector<Client*> filteredClientList;
    std::copy_if (m_clientList.begin(), m_clientList.end(), std::back_inserter(filteredClientList), 
        [](Client* client) { return client->isInsideClub; } );

    std::vector<Event*> tempEvents;
    for (size_t i = 0; i < filteredClientList.size(); i++)
    {
        if (filteredClientList[i]->occupiedTable)
        {
            filteredClientList[i]->tableUsageSessions.back()->endTime = m_config.endTime;
            filteredClientList[i]->occupiedTable = false;
            m_tables[filteredClientList[i]->tableUsageSessions.back()->tableId - 1]->isBusy = false;
        }
        // сгенерировать событие 11 для каждого клиента
        Event* event = CreateEvent(m_config.endTime, 11, filteredClientList[i]->name, 0);
        // вставить событие во временный вектор
        tempEvents.push_back(event);
        // пометить флаг что клиент ушел из клуба
        filteredClientList[i]->isInsideClub = false;
    }
    // отсортировать полученный вектор в алфавитном порядке имен клиентов
    std::sort(tempEvents.begin(), tempEvents.end(), 
        [] (Event* left, Event* right) { return left->client->name < right->client->name; } );

    // вставить элементы временного вектора в m_generatedEvents
    m_generatedEvents.insert(m_generatedEvents.end(), tempEvents.begin(), tempEvents.end());
}

std::vector<Table*> TrackingSystem::GetTables()
{
    return m_tables;
}

std::vector<Event*> TrackingSystem::GetEvents()
{
    return m_generatedEvents;
}

Event* TrackingSystem::CreateEvent(int time, int id, const std::string& clientName, int tableId)
{
    Event* event = new Event();
    event->time = time;
    event->id = id;

    Client* client = new Client();
    client->name = clientName;
    event->client = client;

    event->tableId = tableId;
    return event;
}

Event* TrackingSystem::CreateErrorEvent(int time, const std::string& errorName)
{
    Event* event = new Event();
    event->time = time;
    event->id = 13;
    event->error.name = errorName;
    return event;
}

void TrackingSystem::CalculateIncome()
{
    for (auto &&table : m_tables)
    {
        int usageTime = 0;
        for (auto &&client : m_clientList)
        {
            for (size_t i = 0; i < client->tableUsageSessions.size(); i++)
            {
                if (client->tableUsageSessions[i]->tableId == table->id)
                {
                    usageTime += client->tableUsageSessions[i]->endTime - client->tableUsageSessions[i]->startTime;
                }
            }
        }
        
        table->usageTime = usageTime;
        table->income = std::ceil(usageTime / 60.0) * m_config.hourlyCost;
    }
}