#pragma once

#include "common/types.h"

class Console
{
public:
    static void Output(
        const CompClubConfig& config,
        const std::vector<std::unique_ptr<Event>>& events,
        const std::vector<std::unique_ptr<Table>>& tables
    );

private:
    static std::string FormatTime(int time);
};