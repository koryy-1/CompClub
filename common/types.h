#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>

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

enum EventError
{
    YouShallNotPass,
    NotOpenYet,

    PlaceIsBusy,
    ClientUnknown,
    
    ICanWaitNoLonger,

    // новые ошибки
    ClientHasAlreadyGone,
    WhyGetOnWaitingList
};

struct CompClubConfig
{
    uint32_t tableCount;
    uint32_t hourlyCost;
    int startTime;
    int endTime;
};

struct TableUsageSession
{
    TableUsageSession() {}
    TableUsageSession(int id, int startT, int endT)
        : tableId(id), startTime(startT), endTime(endT) {}

    int tableId;
    int startTime;
    int endTime;
};

struct Client
{
    std::string name;
    bool isInsideClub; // флаг что клиент ушел из клуба
    uint32_t occupiedTableId; // номер стола, занятого клиентом, если клиент не занимает стол, то = 0
};

struct Event
{
    int id;
    int time;
    EventError eventError;
    std::string clientName;
    uint32_t tableId;
};

struct Table
{
    int id;
    bool isBusy;
    int income;
    int usageTime;
    TableUsageSession usageSession;
};
