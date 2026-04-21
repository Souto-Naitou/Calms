#pragma once

#include <Features/RandomGenerator/RandomGenerator.h>
#include <drawable/line/Line.h>
#include <Utility/PathResolver/PathResolver.h>
#include <Utility/JSONIO/JSONIO.h>
#include <DebugTools/DebugEntry/DebugEntry.h>
#include <Features/TimeMeasurer/DeltaTimeStopWatch.h>
#include <entity/enemy/EnemyFactory.h>
#include <entity/enemy/EnemyRepository.h>
#include <Range.h>
#include <nlohmann/json.hpp>
#include <Vector3.h>
#include <string>
#include <memory>

/// <summary>
/// 敵生成クラス
/// 出現範囲などを設定し、敵の自動生成を行います。
/// </summary>
class EnemySpawner
{
public:
    struct PopData
    {
        std::string name;
        double beginTime;
        double endTime;
        std::string enemyType;
        int enemyCount;
        double interval;
    };

public:
    /// <summary>
    /// 敵生成システムを初期化します。
    /// 生成範囲やJSON設定の読み込みなどを行います。
    /// </summary>
    void    Initialize(EnemyRepository* pRepository, EnemyFactory* pFactory);

    /// <summary>
    /// 終了処理を行います。
    /// リソースの解放などを想定しています。
    /// </summary>
    void    Finalize();

    /// <summary>
    /// 生成タイミングや内部状態の更新を行います。
    /// </summary>
    void    Update();

    /// <summary>
    /// ImGui のデバッグウィンドウを描画します。
    /// </summary>
    void    ImGui();

    /// <summary>
    /// 生成範囲や除外範囲の可視化描画を行います。
    /// </summary>
    void    DrawArea();

    /// <summary>
    /// 自動生成を開始します。
    /// </summary>
    void    StartPop();

    /// <summary>
    /// 自動生成を停止します。
    /// </summary>
    void    StopPop();
    bool    IsEnablePop() const { return isEnablePop_; }

    /// Setter
    void    SetPopInterval(float interval) { popInterval_ = interval; }
    void    SetPopCount(uint32_t count) { popCount_ = count; }
    void    SetPopRange(const Range<Vector3>& range) { popRange_ = range; }

    void    SetIgnorePosition(const Vector3& position) { ignorePosition_ = position; }
    void    SetIgnoreRange(float radius) { ignoreRange_ = radius; }
    

private:
    // Internal functions
    /// <summary>
    /// ランダムレンジから位置を決めてポップ要求を生成します。
    /// </summary>
    void PopRandom();  // ランダム生成

    /// <summary>
    /// JSON から読み込んだデータを内部ポップデータに変換します。
    /// </summary>
    void InitPopData();

    /// <summary>
    /// ポップスケジュールの進行・遅延処理などの更新を行います。
    /// </summary>
    void UpdatePop();

    /// <summary>
    /// JSON 設定を再読み込みします。
    /// </summary>
    void ReloadJsonData();

    // Common methods
    using json = nlohmann::json;
    DeltaTimeStopWatch        timerOverall_           = {};                   // !< 全体用タイマー
    DeltaTimeStopWatch        timerPop_               = {};                   // !< 生成用タイマー
    DeltaTimeStopWatch        timerPopDelay_          = {};                   // !< 遅延生成用タイマー
    float                   popInterval_            = 1.0f;                 // !< 生成間隔
    uint32_t                popCount_               = 1;                    // !< 一度に生成する数
    uint32_t                popDelayCount_          = 0;                    // !< 遅延生成する数
    bool                    isEnablePop_            = false;                // !< 生成フラグ

    /// Json
    json                    jsonPopTimeTable_       = {};                   // !< Jsonデータ
    PathResolver            pathResolver_           = {};                   // !< ファイルパス検索
    std::vector<PopData>    popData_                = {};                   // !< 生成データ
    size_t                  popDataIndex_           = 0;                    // !< 生成データのインデックス

    /// ランダム生成の範囲
    Range<Vector3>          popRange_               = {};                   // !< 生成範囲

    /// 除外する位置と範囲
    Vector3                 ignorePosition_         = {};                   // !< 除外範囲 - 中心
    float                   ignoreRange_            = 0.0f;                 // !< 除外範囲 - 半径

    /// デバッグ用
    std::unique_ptr<DebugEntry<EnemySpawner>> pDebugEntry_ = {};            // !< デバッグエントリ
    std::unique_ptr<Line>   linesArea_              = {};                   // !< エリアライン
    std::unique_ptr<Line>   linesIgnoreCircle_      = {};                   // !< 禁止エリアライン
    bool                    isDisplayArea_          = false;                // !< 生成範囲表示フラグ

private:
    EnemyRepository*        pEnemyRepository_       = nullptr;              // !< 敵リポジトリ
    EnemyFactory*           pEnemyFactory_          = nullptr;              // !< 敵生成ファクトリー
    RandomGenerator*        randomGenerator_        = nullptr;              // !< ランダム生成器
    JSONIO*                 jsonIO_                 = nullptr;              // !< Json入出力
};