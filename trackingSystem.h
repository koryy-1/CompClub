#pragma once

#include <queue>
#include "common/types.h"

class TrackingSystem
{
public:
    TrackingSystem(CompClubConfig config, std::vector<Event*> events);
    ~TrackingSystem();

    void Handle();

    std::vector<Table*> GetTables();
    std::vector<Event*> GetEvents();

private:
    Event* CreateEvent(int time, int id, const std::string& clientName, int tableId);
    Event* CreateErrorEvent(int time, const std::string& errorName);
    void CalculateIncome();

    CompClubConfig m_config;
    std::vector<Event*> m_events;
    std::vector<Event*> m_generatedEvents;
    std::vector<Table*> m_tables; // в m_tables index является номером стола (id)

    std::vector<Client*> m_clientList; // список клиентов
    std::queue<Client*> m_queue; // очередь клиентов
};