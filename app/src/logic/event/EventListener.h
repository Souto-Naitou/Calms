#pragma once

#include <unordered_map>
#include <string>

/// <summary>
/// イベント受信クラス (WIP)
/// </summary>
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

    /// <summary>
    /// イベントリスナーを初期化します。
    /// 必要なイベントテーブルのセットアップを行います。
    /// </summary>
    void Initialize();

private:
    EventListener() = default;
    ~EventListener() = default;


private:
    std::unordered_map<std::string, uint32_t> eventMap_;
};