#pragma once

#include <queue>
#include "common/types.h"

class TrackingSystem
{
public:
    TrackingSystem(const CompClubConfig& config, std::vector<std::unique_ptr<Event>> events);

    const std::vector<std::unique_ptr<Table>>& GetTables() const;
    const std::vector<std::unique_ptr<Event>>& GetEvents() const;

private:
    void InitializeTables();

    void Handle();
    void HandleClientArrival(const Event& curEvent);
    void HandleClientSatDownAtTable(const Event& curEvent);
    void HandleClientIsWaiting(const Event& curEvent);
    void HandleClientIsGone(const Event& curEvent);
    void HandleClosingOfClub();

    void CalculateIncome();
    
    std::unique_ptr<Event> CreateEvent(int time, int id, const std::string& clientName, int tableId);
    std::unique_ptr<Event> CreateErrorEvent(int time, EventError eventError);

    CompClubConfig m_config;
    std::vector<std::unique_ptr<Event>> m_events;
    std::vector<std::unique_ptr<Event>> m_generatedEvents;
    
    std::vector<std::unique_ptr<Table>> m_tables; // в m_tables index является номером стола (id)
    std::vector<std::unique_ptr<TableUsageSession>> m_tableUsageSessions;

    std::vector<std::shared_ptr<Client>> m_clientList; // список клиентов
    std::queue<std::shared_ptr<Client>> m_queue; // очередь клиентов
};