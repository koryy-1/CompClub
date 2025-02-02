#include <algorithm>
#include "trackingSystem.h"

TrackingSystem::TrackingSystem(CompClubConfig config, std::vector<Event*> events)
    : m_config(config), m_events(events)
{
    Handle();
}

void TrackingSystem::Handle()
{
    // на каджой итерации идет копирование (мб лучше перемещать?) события 
    // из m_events в m_generatedEvents с генерацией новых событий
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
            else if (FindIndex(m_clientList, m_events[i]->client) != -1)
            {
                Event* event = CreateErrorEvent(m_events[i]->time, "YouShallNotPass");
                m_generatedEvents.push_back(event);
            }
            else
            {
                // тут сначала проверить что клиент уже есть в списке 
                // (на случай если 1 и тот же клиент приходит несколько раз в день)
                m_events[i]->client->isInsideClub = true;
                m_clientList.push_back(m_events[i]->client);
            }
            break;
        }
        case 2:
        {
            // auto iterClient = std::find_if(m_admissionList.begin(), m_admissionList.end(), 
            //     [&](Client* client) { return client->name == m_events[i]->client->name; } );
            // int index = iterClient - m_admissionList.begin();
            int clientIndex = FindIndex(m_clientList, m_events[i]->client);
            if (clientIndex == -1)
            {
                Event* event = CreateErrorEvent(m_events[i]->time, "ClientUnknown");
                m_generatedEvents.push_back(event);
                // здесь нужно учитывать что клиент может несколько раз приходить в комп клуб
                // а надо ли
                break;
            }
            auto iterTable = std::find_if(m_tables.begin(), m_tables.end(), 
                [&](Table* table) { return table->id == m_events[i]->tableId && !table->isBusy; } );
                
            int tableIndex = iterTable - m_tables.begin();
            if (iterTable == m_tables.end()) // if not found
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
                    m_tables[m_clientList[clientIndex]->tableUsageSessions.back()->tableId]->isBusy = false;
                    m_tables[tableIndex]->isBusy = true;
                    // todo: учесть скок времени юзался предыдущий стол
                    // todo: придется учитывать скок времени за каким столом клиент провел
                    m_clientList[clientIndex]->tableUsageSessions.back()->endTime = m_events[i]->time;

                    TableUsageSession* session = new TableUsageSession();
                    session->startTime = m_events[i]->time;
                    session->tableId = m_events[i]->tableId;
                    m_clientList[clientIndex]->tableUsageSessions.push_back(session);
                }
                else
                {
                    m_tables[tableIndex]->isBusy = true;

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
            int clientIndex = FindIndex(m_clientList, m_events[i]->client);
            if (clientIndex == -1)
            {
                Event* event = CreateErrorEvent(m_events[i]->time, "ClientUnknown");
                m_generatedEvents.push_back(event);
                break;
            }
            auto iterTable = std::find_if(m_tables.begin(), m_tables.end(), 
                [&](Table* table) { return table->id == m_events[i]->tableId && !table->isBusy; } );
            if (iterTable != m_tables.end()) // if not found
            {
                Event* event = CreateErrorEvent(m_events[i]->time, "ICanWaitNoLonger!");
                m_generatedEvents.push_back(event);
                break;
            }
            if (m_queue.size() == m_config.tableCount)
            {
                Event* event = CreateEvent(m_events[i]->time, 11, m_events[i]->client->name, 0);
                m_generatedEvents.push_back(event);
                // пометить флаг что клиент ушел из клуба
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
            int clientIndex = FindIndex(m_clientList, m_events[i]->client);
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
                    m_tables[m_clientList[clientIndex]->tableUsageSessions.back()->tableId]->isBusy = false;
                }
                else
                {
                    Client* client = m_queue.front();
                    int clientFromQueueIndex = FindIndex(m_clientList, client); // мб поиск с условием
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

    // гдето мув семантика должна идти, перемещение указателя на Client с vector на queue
}

std::vector<Table*> TrackingSystem::GetTables()
{
    return m_tables;
}

std::vector<Event*> TrackingSystem::GetEvents()
{
    return m_generatedEvents;
}

template <typename T>
int TrackingSystem::FindIndex(const std::vector<T>& container, T value)
{
    auto it = std::find(container.begin(), container.end(), value);
    if (it != container.end())
        return it - container.begin();
    
    return -1;
}

// template <typename T>
// int TrackingSystem::FindIndex(const std::vector<T>& container, std::function<bool(T)> condition)
// {
//     auto it = std::find_if(container.begin(), container.end(), condition);
//     if (it != container.end())
//         return it - container.begin();
    
//     return -1;
// }

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
    for (size_t i = 0; i < m_config.tableCount; i++)
    {
        Table* table = new Table();
        table->id == i + 1;
        // table->income = CalcIncome();
        // table->usageTime = CalcUsageTime();
        m_tables[i] = table;
    }
}