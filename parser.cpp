#include <sstream>
#include <iostream>
#include "parser.h"
#include "common/types.h"

Parser::Parser(const std::vector<std::string>& content) : m_rawData(content)
{
    ParseRawData();
}

void Parser::ParseRawData()
{
    if (m_rawData.empty())
        throw std::runtime_error("Ошибка: файл пуст");
    
    ParseConfig();
    
    ParseEvents();
}

const CompClubConfig& Parser::GetConfig() const
{
    return m_config;
}

std::vector<std::unique_ptr<Event>> Parser::GetEvents()
{
    return std::move(m_events);
}

void Parser::ParseEvents()
{
    for (size_t i = 3; i < m_rawData.size(); i++)
    {
        std::vector<std::string> fields = SplitStringIntoFields(m_rawData[i]);

        if (fields.size() < 3 || (stoi(fields[1]) == 2 && fields.size() < 4))
            throw std::runtime_error("Ошибка: недостаточно данных в строке " + std::to_string(i + 1) + ": " + m_rawData[i]);

        // тут будет создание соотв дочернего события согласно id
        // знач заносить в отдельные функции
        std::unique_ptr<Event> event = std::make_unique<Event>();
        event->time = ParseTime(fields[0]);
        event->id = stoi(fields[1]);
        event->clientName = fields[2];

        if (event->time > m_config.endTime)
            throw std::runtime_error("Ошибка: время события превышает время закрытия клуба в строке " + std::to_string(i + 1) + ": " + m_rawData[i]);

        if (event->id < 1 || event->id > 4)
            throw std::runtime_error("Ошибка: некорректный номер события в строке " + std::to_string(i + 1) + ": " + m_rawData[i]);

        if (!IsValidClientName(event->clientName))
            throw std::runtime_error("Ошибка: некорректное имя клиента в строке " + std::to_string(i + 1) + ": " + m_rawData[i]);

        if (event->id == 2) {
            unsigned long long tableId = std::stoull(fields[3]);
            if (tableId < 1 || tableId > m_config.tableCount)
                throw std::runtime_error("Ошибка: некорректный tableId в строке " + std::to_string(i + 1) + ": " + m_rawData[i]);
            
            event->tableId = static_cast<uint32_t>(tableId);
        }

        if (!m_events.empty() && event->time < m_events.back()->time)
            throw std::runtime_error("Ошибка: события идут не по порядку во времени в строке " + std::to_string(i + 1) + ": " + m_rawData[i]);
        
        m_events.push_back(std::move(event));
    }
}

void Parser::ParseConfig()
{
    unsigned long long tableCount = std::stoull(m_rawData[0]);
    if (tableCount < 1 || tableCount > UINT32_MAX)
        throw std::runtime_error("Ошибка: tableCount должен быть в диапазоне [1 - UINT32_MAX]");
    
    m_config.tableCount = static_cast<uint32_t>(tableCount);

    
    std::istringstream iss(m_rawData[1]);
    std::string startTime, endTime;
    if (!(iss >> startTime >> endTime))
        throw std::runtime_error("Ошибка: неверный формат строки времени");
    
    m_config.startTime = ParseTime(startTime);
    m_config.endTime = ParseTime(endTime);

    if (m_config.startTime >= m_config.endTime)
        throw std::runtime_error("Ошибка: startTime должен быть меньше endTime");

    
    unsigned long long hourlyCost = std::stoull(m_rawData[2]);
    if (hourlyCost > UINT32_MAX)
        throw std::runtime_error("Ошибка: hourlyCost выходит за пределы uint32_t");
    
    m_config.hourlyCost = static_cast<uint32_t>(hourlyCost);
}

bool Parser::IsValidClientName(const std::string& name) {
    for (char c : name)
    {
        if (
            !((c >= 'a' && c <= 'z')
            || (c >= '0' && c <= '9')
            || c == '_'
            || c == '-')
        )
            return false;
    }

    return !name.empty();
}

const std::vector<std::string> Parser::SplitStringIntoFields(const std::string& line)
{
    std::istringstream iss(line);
    std::vector<std::string> fields;
    std::string subs;
    while (std::getline(iss, subs, ' '))
        fields.push_back(subs);

    return fields;
}

int Parser::ParseTime(const std::string& timeStr)
{
    std::istringstream iss(timeStr);
    int hours, minutes;
    char colon;
    if (
        !(iss >> hours >> colon >> minutes)
        || colon != ':'
        || hours < 0 || hours > 23
        || minutes < 0 || minutes > 59
    )
        throw std::runtime_error("Ошибка: неверный формат времени " + timeStr);
    
    return hours * 60 + minutes;
}