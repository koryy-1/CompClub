#pragma once

#include <string>
#include <vector>
#include <memory>

enum EventId
{
    ClientArrival = 1,
    ClientSatDownAtTable = 2,
    ClientIsWaiting = 3,
    ClientIsGone = 4,
    ClientIsGoneGenerated = 11,
    ClientSatDownAtTableGenerated = 12,
    ErrorOccurred = 13,
};

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
    std::vector<std::unique_ptr<TableUsageSession>> tableUsageSessions;
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
    std::string clientName;
    int tableId; 
};

struct EventLog
{
    CompClubConfig config;
    std::vector<std::unique_ptr<Event>> events;
};

struct Table
{
    int id;
    bool isBusy;
    int income;
    int usageTime;
};
