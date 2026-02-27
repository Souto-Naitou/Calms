#include "EnemySpawner.h"

#include <imgui.h>
#include <Utility/JSONIO/JSONIO.h>
#include <Features/DeltaTimeManager/DeltaTimeManager.h>
#include "entity/enemy/EnemyInitParams.h"
#include <utility>

void EnemySpawner::Initialize(EnemyRepository* pRepository, EnemyFactory* pFactory)
{
    /// インスタンスの取得
    randomGenerator_ = RandomGenerator::GetInstance();
    jsonIO_ = JSONIO::GetInstance();
    pEnemyRepository_ = pRepository;
    pEnemyFactory_ = pFactory;

    /// JSONファイルの読み込み
    pathResolver_.Initialize();
    // 検索パスの追加
    pathResolver_.AddSearchPath("Resources/Json");
    // 読み込み
    jsonPopTimeTable_ = JSONIO::GetInstance()->Load(pathResolver_.GetFilePath(kJsonFileName_));

    /// ポップデータの初期化
    this->InitPopData();

    /// デバッグウィンドウを登録
    pDebugEntry_ = std::make_unique<DebugEntry<EnemySpawner>>("EnemySpawner", "EnemySpawner", this, false);

    /// ラインの初期化
    linesArea_ = std::make_unique<Line>(4);
    linesArea_->Initialize();
    linesArea_->SetColor(Vector4(1.0f, 1.0f, 0.0f, 1.0f));

    linesIgnoreCircle_ = std::make_unique<Line>(16);
    linesIgnoreCircle_->Initialize();
    linesIgnoreCircle_->SetColor(Vector4(1.0f, 0.0f, 0.0f, 1.0f));
}

void EnemySpawner::Finalize()
{   
    linesArea_->Finalize();
    linesIgnoreCircle_->Finalize();
}

void EnemySpawner::Update()
{
    if (isEnablePop_ == false)
    {
        return;
    }

    this->UpdatePop();

    if (timerPop_.GetNow<float>() > popInterval_)
    {
        popDelayCount_ = popCount_;
        timerPopDelay_.Start();

        timerPop_.Reset();
        timerPop_.Start();
    }

    if ( timerPopDelay_.GetNow<float>() > 0.2f )
    {
        timerPopDelay_.Reset();
        timerPopDelay_.Start();
        if ( popDelayCount_ > 0 )
        {
            this->PopRandom();
            popDelayCount_--;
        }
    }

    timerOverall_.Update(static_cast<uint32_t>(DeltaTimeChannelReserved::Game));
    timerPop_.Update(static_cast<uint32_t>(DeltaTimeChannelReserved::Game));
    timerPopDelay_.Update(static_cast<uint32_t>(DeltaTimeChannelReserved::Game));
}

void EnemySpawner::DrawArea()
{
    if (isDisplayArea_ == false)
    {
        return;
    }

    /// エリア
    (*linesArea_)[0] = popRange_.start;
    (*linesArea_)[1] = { popRange_.end.x, popRange_.start.y, popRange_.start.z };
    (*linesArea_)[2] = { popRange_.end.x, popRange_.start.y, popRange_.start.z };
    (*linesArea_)[3] = popRange_.end;
    (*linesArea_)[4] = popRange_.end;
    (*linesArea_)[5] = { popRange_.start.x, popRange_.end.y, popRange_.end.z };
    (*linesArea_)[6] = { popRange_.start.x, popRange_.end.y, popRange_.end.z };
    (*linesArea_)[7] = popRange_.start;

    /// 除外エリア (ignoreRangeは半径) 16本の線で円を描く
    float theta = 0;
    Vector2 resultLine[16] = {};

    for (auto& line : resultLine)
    {
        theta += 2.0f * 3.141592f / 16;
        line.x = ignorePosition_.x + std::cosf(theta) * ignoreRange_;
        line.y = ignorePosition_.z + std::sinf(theta) * ignoreRange_;
    }

    for (size_t i = 0; i < 16; i++)
    {
        Vector2 begin = resultLine[i];
        Vector2 end = resultLine[(i + 1) % 16];
        (*linesIgnoreCircle_)[i * 2] = Vector3(begin.x, 0.1f, begin.y);
        (*linesIgnoreCircle_)[(i * 2 + 1) % 16] = Vector3(end.x, 0.1f, end.y);
    }

    linesArea_->Update();
    linesArea_->Draw1F();

    linesIgnoreCircle_->Update();
    linesIgnoreCircle_->Draw1F();
}

void EnemySpawner::StartPop()
{
    timerOverall_.Start();
    timerPop_.Start();
    isEnablePop_ = true;
}

void EnemySpawner::StopPop()
{
    timerOverall_.Reset();
    timerPop_.Reset();
    isEnablePop_ = false;
}

void EnemySpawner::PopRandom()
{
    Vector3 randPosition = {};

    while (true)
    {
        /// ランダム生成
        randPosition.x = randomGenerator_->Generate(popRange_.start.x, popRange_.end.x);
        randPosition.y = randomGenerator_->Generate(popRange_.start.y, popRange_.end.y);
        randPosition.z = randomGenerator_->Generate(popRange_.start.z, popRange_.end.z);

        /// 除外位置との距離を計算
        if (ignoreRange_ > 0.0f)
        {
            float distance = (randPosition - ignorePosition_).LengthWithoutRoot();
            // 除外範囲内ならもう一度
            if (distance < ignoreRange_ * ignoreRange_) continue;
            // 範囲外ならループを抜ける
            break;
        }
    }

    pEnemyFactory_->SetPosition(randPosition);
    auto pEnemy = pEnemyFactory_->Create(EnemyTypeFromString(popData_[popDataIndex_].enemyType));
    pEnemy->Initialize(false);
    pEnemyRepository_->Push(std::move(pEnemy));
}

void EnemySpawner::ImGui()
{
#ifdef _DEBUG
    ImGui::Text("Overall Time: %.1f", timerOverall_.GetNow<float>());
    ImGui::Text(popData_[popDataIndex_].name.c_str());
    ImGui::Text("Pop Time: %.2f", timerPop_.GetNow<float>());

    if (ImGui::Button("Reload PopTimeTable"))
    {
        /// Jsonファイルの再読み込み
        this->ReloadJsonData();
    }

    ImGui::Separator();

    ImGui::Checkbox("Enable", &isEnablePop_);
    ImGui::Checkbox("Display Area", &isDisplayArea_);
    ImGui::InputFloat("Pop Interval", &popInterval_);
    ImGui::InputInt("Pop Count", reinterpret_cast<int*>(&popCount_));
    ImGui::DragFloat3("Pop Range Begin", &popRange_.start.x, 0.01f);
    ImGui::DragFloat3("Pop Range End", &popRange_.end.x, 0.01f);
    ImGui::DragFloat3("Ignore Position", &ignorePosition_.x, 0.01f);
    ImGui::DragFloat("Ignore Range", &ignoreRange_, 0.01f);
#endif
}

void EnemySpawner::InitPopData()
{
    json& enemyTimeTable = jsonPopTimeTable_["Enemy"]["TimeTable"];

    popData_.clear();
    popData_.reserve(enemyTimeTable.size());

    /// タイムテーブルの配列からデータを取得
    for (auto& time : enemyTimeTable)
    {
        PopData data = {};

        data.name = time["name"];
        data.beginTime = time["beginTime"];
        data.endTime = time["endTime"];
        data.enemyType = time["enemyType"];
        data.enemyCount = time["enemyCount"];
        data.interval = time["interval"];

        popData_.push_back(data);
    }

    /// beginTimeでソート
    std::sort(popData_.begin(), popData_.end(), [](const PopData& a, const PopData& b) { return a.beginTime < b.beginTime; });

    /// 各種パラメータ初期化
    popInterval_ = static_cast<float>(popData_.front().interval);
    popCount_ = popData_.front().enemyCount;
    popDataIndex_ = 0;
}

void EnemySpawner::UpdatePop()
{
    if (timerOverall_.GetIsStart() == false)
    {
        return;
    }

    double nowTime = timerOverall_.GetNow<float>();

    /// 終了時間を過ぎたら次のデータへ
    if (popData_[popDataIndex_].endTime < nowTime)
    {
        popDataIndex_++;
        /// 終端に達したら最初に戻す（ループ）
        if (popDataIndex_ >= popData_.size())
        {
            /// TODO:   ループの場合とそうでない場合で処理を変える
            ///         全Waveが終了したらフラグを立ててゲームクリア等が可能

            popDataIndex_ = 0;
            timerOverall_.Reset();
            timerOverall_.Start();
        }

        popInterval_ = static_cast<float>(popData_[popDataIndex_].interval);
        popCount_ = popData_[popDataIndex_].enemyCount;
    }
}

void EnemySpawner::ReloadJsonData()
{
    auto path = pathResolver_.GetFilePath(kJsonFileName_);
    jsonPopTimeTable_ = jsonIO_->Unload(path);
    jsonPopTimeTable_ = jsonIO_->Load(path);
    this->InitPopData();
    timerOverall_.Reset();
    timerOverall_.Start();
}
