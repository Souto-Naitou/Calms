#pragma once

#include <Network/TCP_IP.h>
#include <nlohmann/json.hpp>
#include <Vector3.h>
#include <Utility/JSONIO/JSONIO.h>
#include <queue>
#include <thread>
#include <string>

/// <summary>
/// マルチデータ解決クラス
/// </summary>
class MultiDataResolver
{
public:
    /// <summary>
    /// ネットワーク解決器を初期化します。
    /// 必要なバッファや通信設定の準備を行います。
    /// </summary>
    void    Initialize();

    /// <summary>
    /// 受送信処理スレッドを開始します。
    /// </summary>
    void    Start();

    /// <summary>
    /// スレッド停止と後始末を行います。
    /// </summary>
    void    Finalize();

public:
    /// Setter
    void    SetPlayerPosition(Vector3 _pos)    { position_Player_ = _pos; };
    void    SetNowTime(uint32_t _time)         { nowGameTime_ = _time; };
    
    /// Getter
    bool        isHost() const              { return hostOrClient_ == 0; };

    /// <summary>
    /// 通信相手（Player2）の位置をキューから取り出します。
    /// </summary>
    /// <returns>最新の位置。キューが空の場合は直近値。</returns>
    Vector3     PopPlayer2Position();
    uint32_t    GetNowTime() const          { return nowGameTime_; };


private:
    using json = nlohmann::json;
    /// スレッド
    std::unique_ptr<std::thread>                networkThread_      = nullptr;
    bool                                        threadStopRequest_  = false;        /// スレッド停止リクエスト


    /// TCP/IP
    TCP::Host*                                  host_               = nullptr;
    TCP::Client*                                client_             = nullptr;
    int                                         hostOrClient_       = 0;            /// 0:Host, 1:Client


    /// バッファ設定
    uint32_t                                    queueBuffer_        = 10u;          /// キューのバッファサイズ

    
    /// バッファ情報
    bool                                        qIsNotFull_         = false;        /// キューが満タンでない


    /// 共有データ
    uint32_t                                    nowGameTime_        = 0u;           /// 現在の制限時間


    /// ローカル - 送信データ
    std::string                                 sendBuffer_         = {};           /// 送信バッファ
    json                                        sendJson_           = {};           /// 送信データ
    /// ローカル - ゲームデータ
    Vector3                                     position_Player_    = {};           /// プレイヤー座標

    
    /// リモート - 受信データ
    std::string                                 receiveBuffer_      = {};           /// 受信バッファ
    json                                        receiveJson_        = {};           /// 受信データ
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
    void DeserializeJsonData_Host();

    /// <summary>
    /// JSONデータを解析
    /// </summary>
    void DeserializeJsonData_Client();


private:
    std::string name_ = "Network Buffer";
    /// <summary>
    /// ネットワーク状態などのデバッグウィンドウを描画します。
    /// </summary>
    void DebugWindow();
    JSONIO* jsonIO_ = nullptr;  /// JSON入出力用のユーティリティ
};