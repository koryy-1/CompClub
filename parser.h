#pragma once

#include "common/types.h"
#include <string>
#include <vector>

class Parser
{
public:
    static EventLog* Parse(std::vector<std::string> text);
};