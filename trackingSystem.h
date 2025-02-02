#pragma once

#include <queue>
#include <functional>
#include "common/types.h"

class TrackingSystem
{
public:
    TrackingSystem(CompClubConfig config, std::vector<Event*> events);

    void Handle();


    std::vector<Table*> GetTables();
    std::vector<Event*> GetEvents();

private:
    template <typename T>
    int FindIndex(const std::vector<T>& data, T value);

    template <typename T>
    int FindIndex(const std::vector<T>& container, std::function<bool(const T)> condition);
    
    bool Predicate(Table table);
    
    Event* CreateEvent(int time, int id, const std::string& clientName, int tableId);
    Event* CreateErrorEvent(int time, const std::string& errorName);
    void CalculateIncome();

    CompClubConfig m_config;
    std::vector<Event*> m_events;
    std::vector<Event*> m_generatedEvents;
    std::vector<Table*> m_tables;

    std::vector<Client*> m_clientList; // список клиентов
    std::queue<Client*> m_queue; // очередь клиентов
};