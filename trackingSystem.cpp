#include <algorithm>
#include <cmath>
#include "trackingSystem.h"
#include "utils.h"

TrackingSystem::TrackingSystem(const CompClubConfig& config, std::vector<std::unique_ptr<Event>> events)
    : m_config(config), m_events(std::move(events))
{
    InitializeTables();
    
    Handle();

    CalculateIncome();
}

void TrackingSystem::InitializeTables()
{
    for (size_t i = 1; i <= m_config.tableCount; i++)
    {
        std::unique_ptr<Table> table = std::make_unique<Table>();
        table->id = i;
        m_tables.push_back(std::move(table));
    }
}

void TrackingSystem::Handle()
{
    for (size_t i = 0; i < m_events.size(); i++)
    {
        // сначала идет набор клиентов
        Event& curEvent = *m_events[i];
        m_generatedEvents.push_back(std::move(m_events[i]));
        switch (curEvent.id)
        {
        case ClientArrival:
        {
            if (m_config.startTime > curEvent.time)
            {
                std::unique_ptr<Event> event = CreateErrorEvent(curEvent.time, "NotOpenYet");
                m_generatedEvents.push_back(std::move(event));
            }
            else if (Utils::FindClientIndexByName(m_clientList, curEvent.clientName) != -1)
            {
                std::unique_ptr<Event> event = CreateErrorEvent(curEvent.time, "YouShallNotPass");
                m_generatedEvents.push_back(std::move(event));
            }
            else
            {
                std::unique_ptr<Client> client = std::make_unique<Client>();
                client->name = curEvent.clientName;
                client->isInsideClub = true;
                m_clientList.push_back(std::move(client));
            }
            break;
        }
        case ClientSatDownAtTable:
        {
            int clientIndex = Utils::FindClientIndexByName(m_clientList, curEvent.clientName);
            if (clientIndex == -1)
            {
                std::unique_ptr<Event> event = CreateErrorEvent(curEvent.time, "ClientUnknown");
                m_generatedEvents.push_back(std::move(event));
                break;
            }

            if (m_tables[curEvent.tableId - 1]->isBusy)
            {
                std::unique_ptr<Event> event = CreateErrorEvent(curEvent.time, "PlaceIsBusy");
                m_generatedEvents.push_back(std::move(event));
                break;
            }
            else
            {
                // если клиент сменяет стол, то помечаем предыдущий как не занятый
                if (m_clientList[clientIndex]->occupiedTableId != 0)
                {
                    m_tables[m_clientList[clientIndex]->occupiedTableId - 1]->isBusy = false;
                    m_clientList[clientIndex]->occupiedTableId = curEvent.tableId;

                    m_tables[curEvent.tableId - 1]->isBusy = true;
                    m_tables[curEvent.tableId - 1]->usageSession.endTime = curEvent.time;

                    std::unique_ptr<TableUsageSession> session = std::make_unique<TableUsageSession>(
                        curEvent.tableId,
                        m_tables[curEvent.tableId - 1]->usageSession.startTime,
                        m_tables[curEvent.tableId - 1]->usageSession.endTime
                    );
                    m_tableUsageSessions.push_back(std::move(session));

                    m_tables[curEvent.tableId - 1]
                        ->usageSession = TableUsageSession(curEvent.tableId, curEvent.time, 0);
                }
                else
                {
                    m_tables[curEvent.tableId - 1]->isBusy = true;
                    m_clientList[clientIndex]->occupiedTableId = curEvent.tableId;

                    m_tables[curEvent.tableId - 1]
                        ->usageSession = TableUsageSession(curEvent.tableId, curEvent.time, 0);
                }
            }
            break;
        }
        // потом идет набор очереди
        case ClientIsWaiting:
        {
            int clientIndex = Utils::FindClientIndexByName(m_clientList, curEvent.clientName);
            if (clientIndex == -1)
            {
                std::unique_ptr<Event> event = CreateErrorEvent(curEvent.time, "ClientUnknown");
                m_generatedEvents.push_back(std::move(event));
                break;
            }

            if (Utils::FindTableIndex(m_tables, false) != -1)
            {
                std::unique_ptr<Event> event = CreateErrorEvent(curEvent.time, "ICanWaitNoLonger!");
                m_generatedEvents.push_back(std::move(event));
                break;
            }
            if (m_queue.size() == m_config.tableCount)
            {
                std::unique_ptr<Event> event = CreateEvent(curEvent.time, ClientIsGoneGenerated, curEvent.clientName, 0);
                m_generatedEvents.push_back(std::move(event));
                m_clientList[clientIndex]->isInsideClub = false;
                break;
            }
            else
            {
                m_queue.push(m_clientList[clientIndex]);
            }
            break;
        }
        case ClientIsGone:
        {
            int clientIndex = Utils::FindClientIndexByName(m_clientList, curEvent.clientName);
            if (clientIndex == -1)
            {
                std::unique_ptr<Event> event = CreateErrorEvent(curEvent.time, "ClientUnknown");
                m_generatedEvents.push_back(std::move(event));
                break;
            }
            else
            {
                uint32_t curTableId = m_clientList[clientIndex]->occupiedTableId;

                m_tables[curTableId - 1]->usageSession.endTime = curEvent.time;

                std::unique_ptr<TableUsageSession> session = std::make_unique<TableUsageSession>(
                    curTableId,
                    m_tables[curTableId - 1]->usageSession.startTime,
                    m_tables[curTableId - 1]->usageSession.endTime
                );
                m_tableUsageSessions.push_back(std::move(session));

                m_clientList[clientIndex]->occupiedTableId = 0;
                m_clientList[clientIndex]->isInsideClub = false;

                if (m_queue.empty())
                {
                    m_tables[curTableId - 1]->isBusy = false;
                }
                else
                {
                    std::shared_ptr<Client> clientFromQueue = m_queue.front();
                    m_queue.pop();

                    m_tables[curTableId - 1]
                        ->usageSession = TableUsageSession(curTableId, curEvent.time, 0);

                    clientFromQueue->occupiedTableId = curTableId;

                    std::unique_ptr<Event> event = CreateEvent(
                        curEvent.time,
                        ClientSatDownAtTableGenerated,
                        clientFromQueue->name,
                        curTableId
                    );
                    m_generatedEvents.push_back(std::move(event));
                }
            }
            break;
        }
        default:
            break;
        }
    }

    // выгнать всех оставшихся клиентов через 11 событие, при этом отсортировать клиентов в алф порядке
    std::vector<std::shared_ptr<Client>> filteredClientList;
    
    std::copy_if(
        m_clientList.begin(),
        m_clientList.end(),
        std::back_inserter(filteredClientList),
        [](const std::shared_ptr<Client>& client) { return client->isInsideClub; }
    );

    std::vector<std::unique_ptr<Event>> tempEvents;
    for (size_t i = 0; i < filteredClientList.size(); i++)
    {
        if (filteredClientList[i]->occupiedTableId != 0)
        {
            m_tables[filteredClientList[i]->occupiedTableId - 1]->usageSession.endTime = m_config.endTime;

            std::unique_ptr<TableUsageSession> session = std::make_unique<TableUsageSession>(
                filteredClientList[i]->occupiedTableId,
                m_tables[filteredClientList[i]->occupiedTableId - 1]->usageSession.startTime,
                m_tables[filteredClientList[i]->occupiedTableId - 1]->usageSession.endTime
            );
            m_tableUsageSessions.push_back(std::move(session));

            m_tables[filteredClientList[i]->occupiedTableId - 1]->isBusy = false;
            filteredClientList[i]->occupiedTableId = 0;
        }

        // сгенерировать событие 11 для каждого клиента
        std::unique_ptr<Event> event = CreateEvent(m_config.endTime, ClientIsGoneGenerated, filteredClientList[i]->name, 0);

        // вставить событие во временный вектор
        tempEvents.push_back(std::move(event));

        // пометить флаг что клиент ушел из клуба
        filteredClientList[i]->isInsideClub = false;
    }
    
    // отсортировать полученный вектор в алфавитном порядке имен клиентов
    std::sort(tempEvents.begin(), tempEvents.end(), 
        [] (const std::unique_ptr<Event>& left, const std::unique_ptr<Event>& right) {
            return left->clientName < right->clientName; 
        });

    // вставить элементы временного вектора в m_generatedEvents
    m_generatedEvents.insert(
        m_generatedEvents.end(),
        std::make_move_iterator(tempEvents.begin()),
        std::make_move_iterator(tempEvents.end())
    );
}

const std::vector<std::unique_ptr<Table>>& TrackingSystem::GetTables() const
{
    return m_tables;
}

const std::vector<std::unique_ptr<Event>>& TrackingSystem::GetEvents() const
{
    return m_generatedEvents;
}

std::unique_ptr<Event> TrackingSystem::CreateEvent(int time, int id, const std::string& clientName, int tableId)
{
    std::unique_ptr<Event> event = std::make_unique<Event>();
    event->time = time;
    event->id = id;
    event->clientName = clientName;
    event->tableId = tableId;
    return event;
}

std::unique_ptr<Event> TrackingSystem::CreateErrorEvent(int time, const std::string& errorName)
{
    std::unique_ptr<Event> event = std::make_unique<Event>();
    event->time = time;
    event->id = ErrorOccurred;
    event->error.name = errorName;
    return event;
}

void TrackingSystem::CalculateIncome()
{
    for (auto &&table : m_tables)
    {
        int usageTime = 0;
        for (size_t i = 0; i < m_tableUsageSessions.size(); i++)
        {
            if (table->id == m_tableUsageSessions[i]->tableId)
            {
                usageTime += m_tableUsageSessions[i]->endTime - m_tableUsageSessions[i]->startTime;
            }
        }
        
        table->usageTime = usageTime;
        table->income = std::ceil(usageTime / 60.0) * m_config.hourlyCost;
    }
}