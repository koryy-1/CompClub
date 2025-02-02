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
    bool occupiedTable;
    bool isInsideClub;
    std::vector<TableUsageSession*> tableUsageSessions;
};

struct Error
{
    // todo: may be use enum, and how translate enum to string for uotput to console
    std::string name;
};

struct Event
{
    int id;
    int time;
    // todo: how better separate errors and client + tableNum?
    Error error;
    Client* client;
    int tableId; 
};

struct EventLog
{
    CompClubConfig config;
    // todo: use smart pointers
    std::vector<Event*> events;
};

struct Table
{
    int id;
    // int income;
    bool isBusy;
    // std::string activeClient;
};
