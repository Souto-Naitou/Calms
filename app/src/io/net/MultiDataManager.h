#pragma once

#include <Network/TCP_IP.h>

/// <summary>
/// TCPのホストとクライアントのデータを管理するクラス
/// </summary>
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

    /// <summary>
    /// TCP 通信関連のポインタと種別を取得します。
    /// </summary>
    /// <param name="_ppHost">ホストのポインタ格納先（二重ポインタ）。</param>
    /// <param name="_ppClient">クライアントのポインタ格納先（二重ポインタ）。</param>
    /// <param name="_pHostOrClient_">0:Host, 1:Client を返す格納先。</param>
    void GetTCPData(TCP::Host** _ppHost, TCP::Client** _ppClient, int* _pHostOrClient_);

private:
    TCP::Host* host_ = nullptr;
    TCP::Client* client_ = nullptr;

    int hostOrClient_ = 0;
};