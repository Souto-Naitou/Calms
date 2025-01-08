#pragma once

#include <unordered_map>
#include <string>

class EventListener
{
public:
    EventListener(EventListener&&) = delete;
    EventListener& operator=(EventListener&&) = delete;
    EventListener(const EventListener&) = delete;
    EventListener& operator=(const EventListener&) = delete;

    static EventListener* GetInstance()
    {
        static EventListener instance;
        return &instance;
    }

    void Initialize();

private:
    EventListener() = default;
    ~EventListener() = default;


private:
    std::unordered_map<std::string, uint32_t> eventMap_;
};