#pragma once

#include <string>
#include <vector>
#include <ctime>
#include "common/types.h"

class Parser
{
public:
    Parser(const std::vector<std::string>& text);
    
    const CompClubConfig& GetConfig() const;
    std::vector<std::unique_ptr<Event>> GetEvents();

private:
    void ParseRawData();
    void ParseConfig();
    void ParseEvents();
    int ParseTime(const std::string& timeStr);
    const std::vector<std::string> SplitStringIntoFields(const std::string& line);
    bool IsValidClientName(const std::string& name);

    std::vector<std::string> m_rawData;
    CompClubConfig m_config;
    std::vector<std::unique_ptr<Event>> m_events;
};