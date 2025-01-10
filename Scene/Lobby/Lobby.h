#pragma once

#include <Interfaces/IScene.h>
#include <Network/TCP_IP.h>

#include <thread>

#include <JsonParser/JsonLoader.h>


class LobbyScene : public IScene
{
public:
    /// <summary>
    /// 初期化
    /// </summary>
    void Initialize() override;

    /// <summary>
    /// 終了
    /// </summary>
    void Finalize() override;

    /// <summary>
    /// 更新
    /// </summary>
    void Update() override;

    /// <summary>
    /// 背景描画
    /// </summary>
    void Draw2dBackGround() override;

    /// <summary>
    /// 3D描画
    /// </summary>
    void Draw3d() override;

    /// <summary>
    /// 中景描画
    /// </summary>
    void Draw2dMidground() override;

    /// <summary>
    /// 3D中景描画
    /// </summary>
    void Draw3dMidground() override;

    /// <summary>
    /// ライン描画
    /// </summary>
    void DrawLine() override;

    /// <summary>
    /// 前景描画
    /// </summary>
    void Draw2dForeground() override;


private:
    WSADATA wsaData_;
    TCP::Host* host_;
    uint32_t handle_ = 0;


    TCP::Client* client_;

    int hostOrClient_ = 0;

    std::string ip_ = "";
    std::string port_ = "";

    bool hostOpenRequest_ = false;
    bool hostOpened_ = false;

    bool clientTryConnectionRequest_ = false;
    bool clientTryConnection_ = false;

    bool isConnected_ = false;

    std::thread* networkThread_ = nullptr;
    bool isEnableJson_ = false;

private:
    std::string windowName_ = "LobbyScene";
    void DebugWindow();

    /// ネットワークスレッド
    void DeleteNetworkThread();
    void BeginNetworkThread();

    void HostOpen();
    void ClientConnect();

    void HostFinalize();
    void ClientFinalize();
};