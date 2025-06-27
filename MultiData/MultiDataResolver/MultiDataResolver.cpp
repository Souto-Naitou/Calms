#include "MultiDataResolver.h"
#include <MultiDataManager/MultiDataManager.h>
#include <imgui.h>
#include <DebugTools/DebugManager/DebugManager.h>
#include <Utility/JSONConvTypeFuncs/JSONConvTypeFuncs.h>

void MultiDataResolver::Initialize()
{
    MultiDataManager::GetInstance()->GetTCPData(&host_, &client_, &hostOrClient_);

    DebugManager::GetInstance()->SetComponent("Network", name_, std::bind(&MultiDataResolver::DebugWindow, this));
}

void MultiDataResolver::Start()
{
    auto pFunc = [&]()
    {
        while (true)
        {
            if (threadStopRequest_)
            {
                break;
            }

            ::Sleep(16);


            if (hostOrClient_ == 0)
            {
                this->PackageJsonData_Host();
                if (!host_->Send(0, sendBuffer_.c_str(), static_cast<int>(sendBuffer_.size())))
                {
                    ::OutputDebugStringA("Send Error\n");
                    break;
                }

                receiveBuffer_ = host_->Receive(0);
                this->DeserializeJsonData_Host();
            }
            else
            {
                receiveBuffer_ = client_->Receive();
                this->DeserializeJsonData_Client();

                this->PackageJsonData_Client();
                if (!client_->Send(sendBuffer_))
                {
                    ::OutputDebugStringA("Send Error\n");
                    break;
                }
            }
        }
    };


    /// スレッドの生成
    if (networkThread_ == nullptr)
    {
        networkThread_ = std::make_unique<std::thread>(pFunc);
    }
}

void MultiDataResolver::Finalize()
{
    if (networkThread_)
    {
        threadStopRequest_ = true;
        networkThread_->join();
    }

    if (hostOrClient_ == 0)
    {
        host_->Close();
        delete host_;
    }
    else
    {
        client_->Close();
        delete client_;
    }

    DebugManager::GetInstance()->DeleteComponent("Network", name_);
}

Vector3 MultiDataResolver::PopPlayer2Position()
{
    /// キューが空の場合満タンになるまで溜めたい (からフラグを立てる)
    if (qPos_P2.empty())
    {
        qIsNotFull_ = true;
    }

    /// キューのバッファが溜まったらキューの消化を開始
    if (qPos_P2.size() >= queueBuffer_)
    {
        qIsNotFull_ = false;
    }

    /// キューのバッファが溜まりきっていない場合、前回と同じ座標を返す
    if (qIsNotFull_)
    {
        return position_Player2_;
    }

    /// キューのバッファが溜まりきったら空になるまでキューから取り出す
    /// ただし、キューには受信したデータがプッシュされるはず
    else
    {
        position_Player2_ = qPos_P2.front();
        qPos_P2.pop();
        return position_Player2_;
    }
}

void MultiDataResolver::PackageJsonData_Host()
{
    /// ゲームデータをJSONにパッケージング
    json root = {};


    /// [Global]
    json& global = root["Global"];
    global["nowTime"] = nowGameTime_;


    /// [Player]
    json& player = root["Player"];
    player["position"] = position_Player_;


    sendJson_ = root;


    /// 送信データの生成
    sendBuffer_ = sendJson_.dump(4);
}

void MultiDataResolver::PackageJsonData_Client()
{
    /// ゲームデータをJSONにパッケージング
    json root = {};


    /// [Player]
    json& player = root["Player"];
    player["position"] = position_Player_;


    sendJson_ = root;


    /// 送信データの生成
    sendBuffer_ = sendJson_.dump(4);
}

void MultiDataResolver::DeserializeJsonData_Host()
{
    /// デシリアライズ
    receiveJson_ = receiveBuffer_;

    qPos_P2.push(receiveJson_["Player"]["position"]);
}

void MultiDataResolver::DeserializeJsonData_Client()
{
    /// デシリアライズ
    receiveJson_ = receiveBuffer_;

    /// [Global:NowTime]
    nowGameTime_ = receiveJson_["Global"]["nowTime"];

    /// [Player:Position]
    qPos_P2.push(receiveJson_["Player"]["position"]);
}

void MultiDataResolver::DebugWindow()
{
#ifdef _DEBUG

    ImGui::BeginChild("Buffer", ImVec2(-1, -1), ImGuiChildFlags_Border);

    ImGui::ShowDemoWindow();
    ImGui::TextUnformatted(receiveBuffer_.c_str());

    ImGui::EndChild();

#endif // _DEBUG
}
