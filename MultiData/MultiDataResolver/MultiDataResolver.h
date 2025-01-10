#pragma once

#include <Network/TCP_IP.h>
#include <JsonParser/json.h>
#include <Vector3.h>
#include <Timer/Timer.h>
#include <queue>
#include <thread>

class MultiDataResolver
{
public:
    void Initialize();
    void Start();
    void Finalize();

public: /// Setter
    void SetPlayerPosition(Vector3 _pos)    { position_Player_ = _pos; };
    void SetNowTime(uint32_t _time)         { nowGameTime_ = _time; };

public: /// Getter
    bool        isHost() const              { return hostOrClient_ == 0; };

    Vector3     PopPlayer2Position();
    uint32_t    GetNowTime() const          { return nowGameTime_; };


private:
    /// スレッド
    std::thread*                                networkThread_      = nullptr;
    bool                                        threadStopRequest_  = false;        /// スレッド停止リクエスト


    /// TCP/IP
    TCP::Host*                                  host_               = nullptr;
    TCP::Client*                                client_             = nullptr;
    int                                         hostOrClient_       = 0;            /// 0:Host, 1:Client


    /// JSON-Parser
    Json::Parser*                               jsonParser_         = {};
    Json::Saver*                                jsonSaver_          = {};


    /// バッファ設定
    uint32_t                                    queueBuffer_        = 5u;           /// キューのバッファサイズ

    
    /// バッファ情報
    bool                                        qIsNotFull_         = false;        /// キューが満タンでない


    /// 共有データ
    uint32_t                                    nowGameTime_        = 0u;           /// 現在の制限時間


    /// ローカル - 送信データ
    std::string                                 sendBuffer_         = {};           /// 送信バッファ
    Json::Value                                 sendJson_           = {};           /// 送信データ
    /// ローカル - ゲームデータ
    Vector3                                     position_Player_    = {};           /// プレイヤー座標

    
    /// リモート - 受信データ
    std::string                                 receiveBuffer_      = {};           /// 受信バッファ
    Json::Value                                 receiveJson_        = {};           /// 受信データ
    /// リモート - ゲームデータ
    std::queue<Vector3>                         qPos_P2             = {};           /// 通信相手の座標バッファ
    Vector3                                     position_Player2_   = {};           /// ゲームで使用する通信相手の座標


private:
    /// <summary>
    /// ゲームデータをJSONにパッケージング
    /// </summary>
    void PackageJsonData_Host();

    /// <summary>
    /// ゲームデータをJSONにパッケージング
    /// </summary>
    void PackageJsonData_Client();


    /// <summary>
    /// JSONデータを解析
    /// </summary>
    void ParseJsonData_Host();

    /// <summary>
    /// JSONデータを解析
    /// </summary>
    void ParseJsonData_Client();


private:
    std::string name_ = "Network Buffer";
    void DebugWindow();

};