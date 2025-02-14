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
    inline static std::map<EventError, std::string> m_eventErrorMap = {
        { YouShallNotPass, "YouShallNotPass" },
        { NotOpenYet, "NotOpenYet" },
        
        { PlaceIsBusy, "PlaceIsBusy" },
        { ClientUnknown, "ClientUnknown" },
            
        { ICanWaitNoLonger, "ICanWaitNoLonger!" },
        
        // новые ошибки
        { ClientHasAlreadyGone, "ClientHasAlreadyGone" },
        { WhyGetOnWaitingList, "WhyGetOnWaitingList?" }
    };
};