#include "EnemyPopSystem.h"

#include <imgui.h>
#include <DebugTools/DebugManager/DebugManager.h>

void EnemyPopSystem::Initialize()
{
    /// インスタンスの取得
    randomGenerator_ = RandomGenerator::GetInstance();

    /// Jsonファイルの読み込み
    filePathSearcher_.Initialize();
    // 検索パスの追加
    filePathSearcher_.AddSearchPath("Resources/Json");
    // 読み込み
    jsonPopTimeTable_ = jsonLoader_[filePathSearcher_.GetFilePath(kJsonFileName_)];


    /// ポップデータの初期化
    InitPopData();


    /// デバッグウィンドウを登録
    DebugManager::GetInstance()->SetComponent(name_, std::bind(&EnemyPopSystem::DebugWindow, this));


    /// ラインの初期化
    linesArea_ = new Line(4);
    linesArea_->Initialize();
    linesArea_->SetColor(Vector4(1.0f, 1.0f, 0.0f, 1.0f));

    linesIgnoreCircle_ = new Line(16);
    linesIgnoreCircle_->Initialize();
    linesIgnoreCircle_->SetColor(Vector4(1.0f, 0.0f, 0.0f, 1.0f));
}

void EnemyPopSystem::Finalize()
{   
    linesArea_->Finalize();
    delete linesArea_;

    linesIgnoreCircle_->Finalize();
    delete linesIgnoreCircle_;

    DebugManager::GetInstance()->DeleteComponent(name_.c_str());
}

void EnemyPopSystem::Update()
{
    if (isEnablePop_ == false)
    {
        return;
    }

    UpdatePop();

    if (timerPop_.GetNow() > popInterval_)
    {
        for (uint32_t i = 0; i < popCount_; i++)
        {
            PopRandom();
        }

        timerPop_.Reset();
        timerPop_.Start();
    }
}

void EnemyPopSystem::DrawArea()
{
    if (isDisplayArea_ == false)
    {
        return;
    }

    /// エリア
    (*linesArea_)[0] = popRangeBegin_;
    (*linesArea_)[1] = { popRangeEnd_.x, popRangeBegin_.y, popRangeBegin_.z };
    (*linesArea_)[2] = { popRangeEnd_.x, popRangeBegin_.y, popRangeBegin_.z };
    (*linesArea_)[3] = popRangeEnd_;
    (*linesArea_)[4] = popRangeEnd_;
    (*linesArea_)[5] = { popRangeBegin_.x, popRangeEnd_.y, popRangeEnd_.z };
    (*linesArea_)[6] = { popRangeBegin_.x, popRangeEnd_.y, popRangeEnd_.z };
    (*linesArea_)[7] = popRangeBegin_;

    /// 除外エリア (ignoreRangeは半径) 16本の線で円を描く
    float theta = 0;
    Vector2 resultLine[16] = {};

    for (int i = 0; i < 16; i++)
    {
        theta += 2.0f * 3.141592f / 16;
        resultLine[i].x = ignorePosition_.x + std::cosf(theta) * ignoreRange_;
        resultLine[i].y = ignorePosition_.z + std::sinf(theta) * ignoreRange_;
    }

    for (size_t i = 0; i < 16; i++)
    {
        Vector2 begin = resultLine[i];
        Vector2 end = resultLine[(i + 1) % 16];
        (*linesIgnoreCircle_)[i * 2] = Vector3(begin.x, 0.1f, begin.y);
        (*linesIgnoreCircle_)[(i * 2 + 1) % 16] = Vector3(end.x, 0.1f, end.y);
    }

    linesArea_->Update();
    linesArea_->Draw();

    linesIgnoreCircle_->Update();
    linesIgnoreCircle_->Draw();
}


void EnemyPopSystem::ManualPop()
{
    PopRandom();
    return;
}

void EnemyPopSystem::ManualPop(const Vector3& _position)
{
    popPoints_.push(_position);
    return;
}

Vector3 EnemyPopSystem::GetPopPoint()
{
    Vector3 popPoint = popPoints_.front();
    popPoints_.pop();
    return popPoint;
}

void EnemyPopSystem::StartPop()
{
    timerOverall_.Start();
    timerPop_.Start();
    isEnablePop_ = true;
    return;
}

void EnemyPopSystem::StopPop()
{
    timerOverall_.Reset();
    timerPop_.Reset();
    isEnablePop_ = false;
    return;
}

void EnemyPopSystem::PopRandom()
{
    Vector3 randPosition = {};

    bool isValid = true;
    do
    {
        /// ランダム生成
        randPosition.x = randomGenerator_->Generate(popRangeBegin_.x, popRangeEnd_.x);
        randPosition.y = randomGenerator_->Generate(popRangeBegin_.y, popRangeEnd_.y);
        randPosition.z = randomGenerator_->Generate(popRangeBegin_.z, popRangeEnd_.z);

        /// 除外位置との距離を計算
        isValid = true;
        if (ignoreRange_ > 0.0f)
        {
            float distance = (randPosition - ignorePosition_).LengthWithoutRoot();
            if (distance < ignoreRange_ * ignoreRange_)
            {
                isValid = false;
            }
        }
    } while (isValid != true);

    // 生成位置をキューに追加
    popPoints_.push(randPosition);
}

void EnemyPopSystem::DebugWindow()
{
#ifdef _DEBUG
    ImGui::Text("Overall Time: %.1f", timerOverall_.GetNow());
    ImGui::Text(popData_[popDataIndex_].name.c_str());
    ImGui::Text("Pop Time: %.2f", timerPop_.GetNow());

    if (ImGui::Button("Reload PopTimeTable"))
    {
        jsonLoader_.ReloadFile(filePathSearcher_.GetFilePath(kJsonFileName_));
        jsonPopTimeTable_ = jsonLoader_[filePathSearcher_.GetFilePath(kJsonFileName_)];
        InitPopData();
        timerOverall_.Reset();
        timerOverall_.Start();
    }

    ImGui::Separator();

    ImGui::Checkbox("Display Area", &isDisplayArea_);
    ImGui::InputFloat("Pop Interval", &popInterval_);
    ImGui::InputInt("Pop Count", reinterpret_cast<int*>(&popCount_));
    ImGui::DragFloat3("Pop Range Begin", &popRangeBegin_.x, 0.01f);
    ImGui::DragFloat3("Pop Range End", &popRangeEnd_.x, 0.01f);
    ImGui::DragFloat3("Ignore Position", &ignorePosition_.x), 0.01f;
    ImGui::DragFloat("Ignore Range", &ignoreRange_, 0.01f);
#endif
}

void EnemyPopSystem::ModifyGameEye(GameEye* _eye)
{
    linesArea_->SetGameEye(_eye);

    linesIgnoreCircle_->SetGameEye(_eye);
}

void EnemyPopSystem::InitPopData()
{
    Json::Array enemyTimeTable = (jsonPopTimeTable_["Enemy"])["TimeTable"];

    popData_.clear();
    popData_.reserve(enemyTimeTable.size());

    /// タイムテーブルの配列からデータを取得
    for (auto& time : enemyTimeTable)
    {
        PopData data = {};

        data.name = (*time)["name"];
        data.beginTime = (*time)["beginTime"];
        data.endTime = (*time)["endTime"];
        data.enemyType = (*time)["enemyType"];
        data.enemyCount = (*time)["enemyCount"];
        data.interval = (*time)["interval"];

        popData_.push_back(data);
    }

    /// beginTimeでソート
    std::sort(popData_.begin(), popData_.end(), [](const PopData& a, const PopData& b) { return a.beginTime < b.beginTime; });

    /// 各種パラメータ初期化
    popInterval_ = static_cast<float>(popData_.front().interval);
    popCount_ = popData_.front().enemyCount;
    popDataIndex_ = 0;
}

void EnemyPopSystem::UpdatePop()
{
    if (timerOverall_.GetIsStart() == false)
    {
        return;
    }

    double nowTime = timerOverall_.GetNow();

    /// 終了時間を過ぎたら次のデータへ
    if (popData_[popDataIndex_].endTime < nowTime)
    {
        popDataIndex_++;
        /// 終端に達したら最初に戻す（ループ）
        if (popDataIndex_ >= popData_.size())
        {
            /// TODO:   ループの場合とそうでない場合で処理を変える
            ///         ルーチンが終了したらフラグを立ててゲームクリア等が可能

            popDataIndex_ = 0;
            timerOverall_.Reset();
            timerOverall_.Start();
        }

        popInterval_ = static_cast<float>(popData_[popDataIndex_].interval);
        popCount_ = popData_[popDataIndex_].enemyCount;
    }
}
