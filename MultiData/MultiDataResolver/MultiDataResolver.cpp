#include "MultiDataResolver.h"
#include <MultiDataManager/MultiDataManager.h>
#include <Features/Particle/Manager/EmitterManager.h>
#include <imgui.h>
#include <DebugTools/DebugManager/DebugManager.h>

void MultiDataResolver::Initialize()
{
    MultiDataManager::GetInstance()->GetTCPData(&host_, &client_, &hostOrClient_);

    DebugManager::GetInstance()->SetComponent("#Window", name_, std::bind(&MultiDataResolver::DebugWindow, this));

    jsonParser_ = new Json::Parser();
    jsonSaver_ = Json::Saver::GetInstance();
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

            Sleep(16);


            if (hostOrClient_ == 0)
            {
                PackageJsonData_Host();
                if (!host_->Send(0, sendBuffer_.c_str(), static_cast<int>(sendBuffer_.size())))
                {
                    OutputDebugStringA("Send Error\n");
                    break;
                }

                receiveBuffer_ = host_->Receive(0);
                ParseJsonData_Host();
            }
            else
            {
                receiveBuffer_ = client_->Receive();
                ParseJsonData_Client();

                PackageJsonData_Client();
                if (!client_->Send(sendBuffer_))
                {
                    OutputDebugStringA("Send Error\n");
                    break;
                }
            }
        }
    };


    /// スレッドの生成
    if (networkThread_ == nullptr)
    {
        networkThread_ = new std::thread(pFunc);
    }
}

void MultiDataResolver::Finalize()
{
    if (networkThread_)
    {
        threadStopRequest_ = true;
        networkThread_->join();
        delete networkThread_;
        networkThread_ = nullptr;
    }
    delete jsonParser_;

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

    DebugManager::GetInstance()->DeleteComponent("#Window", name_.c_str());
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
    Json::Object root = {};


    /// [Global]
    Json::Object global = {};
    global["nowTime"] = std::make_shared<Json::Value>(Json::Value::Type::Int, static_cast<int>(nowGameTime_));

    root["Global"] = std::make_shared<Json::Value>(Json::Value::Type::Object, global);


    /// [Player]
    Json::Object player = {};
    player["position"] = EmitterManager::ParseVector3(position_Player_);

    root["Player"] = std::make_shared<Json::Value>(Json::Value::Type::Object, player);


    sendJson_.type = Json::Value::Type::Object;
    sendJson_.value = root;

    /// 送信データの生成
    sendBuffer_ = jsonSaver_->GetString(sendJson_, 0, false);
}

void MultiDataResolver::PackageJsonData_Client()
{
    /// ゲームデータをJSONにパッケージング
    Json::Object root = {};

    /// [Player]
    Json::Object player = {};
    player["position"] = EmitterManager::ParseVector3(position_Player_);

    root["Player"] = std::make_shared<Json::Value>(Json::Value::Type::Object, player);


    sendJson_.type = Json::Value::Type::Object;
    sendJson_.value = root;

    /// 送信データの生成
    sendBuffer_ = jsonSaver_->GetString(sendJson_, 0, false);
}

void MultiDataResolver::ParseJsonData_Host()
{
    /// パースして取得
    jsonParser_->Initialize();
    jsonParser_->SetJsonString(receiveBuffer_);
    jsonParser_->Run();
    receiveJson_ = jsonParser_->GetData();

    Json::Value player = receiveJson_["Player"];

    Json::Object pos = player["position"];

    qPos_P2.push(EmitterManager::ParseVector3(pos));
}

void MultiDataResolver::ParseJsonData_Client()
{
    /// パースして取得
    jsonParser_->Initialize();
    jsonParser_->SetJsonString(receiveBuffer_);
    jsonParser_->Run();
    receiveJson_ = jsonParser_->GetData();


    /// [Global]
    Json::Value global = receiveJson_["Global"];

    /// [Global:NowTime]
    nowGameTime_ = static_cast<int>(global["nowTime"]);


    /// [Player]
    Json::Value player = receiveJson_["Player"];

    /// [Player:Position]
    Json::Object pos = player["position"];


    qPos_P2.push(EmitterManager::ParseVector3(pos));
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
