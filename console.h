#pragma once

#include "common/types.h"

class Console
{
public:
    static void Output(
        CompClubConfig config,
        std::vector<Event*> events,
        std::vector<Table*> tables
    );
};