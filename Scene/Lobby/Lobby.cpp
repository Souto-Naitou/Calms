#include "Lobby.h"

#include <stdexcept>
#include <DebugTools/DebugManager/DebugManager.h>
#include <MultiDataManager/MultiDataManager.h>
#include <Features/SceneTransition/SceneTransitionManager.h>
#include <Features/SceneTransition/TransFadeInOut.h>
#include <imgui.h>
#include <Utility/JSONIO/JSONIO.h>

using json = nlohmann::json;

void LobbyScene::Initialize()
{
    /// DebugWindowを登録
    DebugManager::GetInstance()->SetComponent("Scene", windowName_, std::bind(&LobbyScene::DebugWindow, this));


    /// WSADataの初期化
    HRESULT hr = ::WSAStartup(MAKEWORD(2, 0), &wsaData_);
    if (!SUCCEEDED(hr))
    {
        throw std::runtime_error("WSAStartup failed");
        return;
    }

    json connInfo = JSONIO::GetInstance()->Load("Resources/Json/ipaddr.json");

    isEnableJson_ = connInfo["ENABLE"];

    if (!isEnableJson_) return;

    hostOrClient_ = connInfo["HOSTORCLIENT"];
    if (hostOrClient_ == 0)
    {
        port_ = connInfo["PORT"];
        hostOpenRequest_ = true;
    }
    else
    {
        ip_ = connInfo["IPADDR"];
        port_ = connInfo["PORT"];
        clientTryConnectionRequest_ = true;
    }
}

void LobbyScene::Finalize()
{
    if (hostOrClient_ == 0)
    {
        MultiDataManager::GetInstance()->SetTCPData(host_);
    }
    else
    {
        MultiDataManager::GetInstance()->SetTCPData(client_);
    }

    this->DeleteNetworkThread();

    DebugManager::GetInstance()->DeleteComponent("Scene", windowName_.c_str());
}

void LobbyScene::Update()
{
    /// == ネットワークスレッドの開始 ==
    ///     ホストオープンリクエストがあるか、クライアント接続リクエストがあるか
    ///     どちらかがあればスレッドを開始する
    this->BeginNetworkThread();

    if (isConnected_)
    {
        SceneTransitionManager::GetInstance()->ChangeScene("MultiGame", std::make_unique<TransFadeInOut>());
        isConnected_ = false;
    }
}

void LobbyScene::Draw2dBackGround()
{
}

void LobbyScene::Draw3d()
{
}

void LobbyScene::Draw2dMidground()
{
}

void LobbyScene::Draw3dMidground()
{
}

void LobbyScene::DrawLine()
{
}

void LobbyScene::Draw2dForeground()
{
}

void LobbyScene::DrawTexts()
{
}

void LobbyScene::DebugWindow()
{
#ifdef _DEBUG

    if (isConnected_) 
    {
        ImGui::Text("Connected");
        return;
    }

    /// 接続試行中は操作を無効化
    if (hostOpened_ || clientTryConnection_) ImGui::BeginDisabled();


    const char* items[] = { "Host", "Client" };
    ImGui::ListBox("Lobby", &hostOrClient_, items, 2);


    if (hostOrClient_ == 1)
    {
        char ipaddr[17] = {};
        memcpy_s(ipaddr, sizeof(ipaddr), ip_.c_str(), ip_.size());
        if (ImGui::InputText("IP ADDR", ipaddr, sizeof(ipaddr), ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_CharsNoBlank))
        {
            ip_ = ipaddr;
        }
    }


    char portaddr[6] = {};
    memcpy_s(portaddr, sizeof(portaddr), port_.c_str(), port_.size());
    if (ImGui::InputText("PORT", portaddr, sizeof(portaddr), ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_CharsNoBlank))
    {
        port_ = portaddr;
    }


    /// Host
    if (hostOrClient_ == 0)
    {
        if (ImGui::Button("Open"))
        {
            hostOpenRequest_ = true;
        }
    }
    /// Client
    else
    {
        if (ImGui::Button("Connect"))
        {
            clientTryConnectionRequest_ = true;
        }
    }


    if (hostOpened_ || clientTryConnection_) ImGui::EndDisabled();

#endif
}

void LobbyScene::DeleteNetworkThread()
{
    if (networkThread_)
    {
        networkThread_->join();
        delete networkThread_;
        networkThread_ = nullptr;
    }
}

void LobbyScene::BeginNetworkThread()
{
    if (hostOpenRequest_)
    {
        DeleteNetworkThread();
        networkThread_ = new std::thread(&LobbyScene::HostOpen, this);
        hostOpenRequest_ = false;
    }

    if (clientTryConnectionRequest_)
    {
        DeleteNetworkThread();
        networkThread_ = new std::thread(&LobbyScene::ClientConnect, this);
        clientTryConnectionRequest_ = false;
    }
}

void LobbyScene::HostOpen()
{
    HostFinalize();

    host_ = new TCP::Host();
    hostOpened_ = true;

    host_->Initialize(wsaData_);
    host_->Open(port_);
    handle_ = host_->ListenAndAccept();

    isConnected_ = true;
}

void LobbyScene::ClientConnect()
{
    ClientFinalize();

    client_ = new TCP::Client();
    clientTryConnection_ = true;

    client_->Initialize(wsaData_);

    while (!client_->Connect(ip_, port_))
    {
        OutputDebugStringA("Connect failed\n");
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    isConnected_ = true;
}

void LobbyScene::HostFinalize()
{
    if (!host_) return;

    host_->Close();
    delete host_;
    host_ = nullptr;
}

void LobbyScene::ClientFinalize()
{
    if (!client_) return;

    client_->Close();
    delete client_;
    client_ = nullptr;
}
