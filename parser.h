#pragma once

#include <string>
#include <vector>
#include <ctime>
#include "common/types.h"

class Parser
{
public:
    static std::unique_ptr<EventLog> Parse(const std::vector<std::string>& text);

private:
    static int ParseTime(const std::string& timeStr);
};