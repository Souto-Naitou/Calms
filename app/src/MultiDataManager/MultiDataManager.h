#pragma once

#include <Network/TCP_IP.h>

class MultiDataManager
{
public:
    MultiDataManager(const MultiDataManager&) = delete;
    MultiDataManager(const MultiDataManager&&) = delete;
    MultiDataManager& operator=(const MultiDataManager&) = delete;
    MultiDataManager& operator=(const MultiDataManager&&) = delete;

    static MultiDataManager* GetInstance()
    {
        static MultiDataManager instance; return &instance;
    }

private:
    MultiDataManager() = default;
    ~MultiDataManager() = default;

public:
    void SetTCPData(TCP::Host* _host)
    {
        host_ = _host;
        hostOrClient_ = 0;
    }

    void SetTCPData(TCP::Client* _client)
    {
        client_ = _client;
        hostOrClient_ = 1;
    }

    void GetTCPData(TCP::Host** _ppHost, TCP::Client** _ppClient, int* _pHostOrClient_);

private:
    TCP::Host* host_ = nullptr;
    TCP::Client* client_ = nullptr;

    int hostOrClient_ = 0;
};