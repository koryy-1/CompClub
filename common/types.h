#pragma once

#include <string>
#include <vector>

struct CompClubConfig
{
    int tableCount;
    int startTime;
    int endTime;
    int hourlyCost;
};

struct TableUsageSession
{
    int tableId;
    int startTime;
    int endTime;
};

struct Client
{
    std::string name;
    bool occupiedTable; // флаг что клиент занял стол
    bool isInsideClub; // флаг что клиент ушел из клуба
    std::vector<TableUsageSession*> tableUsageSessions;
};

struct Error
{
    std::string name;
};

struct Event
{
    int id;
    int time;
    Error error;
    Client* client;
    int tableId; 
};

struct EventLog
{
    CompClubConfig config;
    std::vector<Event*> events;
};

struct Table
{
    int id;
    bool isBusy;
    int income;
    int usageTime;
};
