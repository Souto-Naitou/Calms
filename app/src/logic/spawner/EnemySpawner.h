#pragma once

#include <Features/RandomGenerator/RandomGenerator.h>
#include <Features/TimeMeasurer/TimeMeasurer.h>
#include <Vector3.h>
#include <Features/GameEye/GameEye.h>
#include <Features/Line/Line.h>
#include <Utility/PathResolver/PathResolver.h>
#include <queue>
#include <string>
#include <Utility/JSONIO/JSONIO.h>
#include <nlohmann/json.hpp>
#include <DebugTools/DebugEntry/DebugEntry.h>
#include <memory>

/// <summary>
/// 敵生成クラス
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
    void    Initialize();

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
    /// 手動で敵のポップ要求を追加します。
    /// 位置はランダムレンジから決定されます。
    /// </summary>
    void    ManualPop();

    /// <summary>
    /// 手動で敵のポップ要求を追加します。
    /// </summary>
    /// <param name="_position">生成位置。</param>
    void    ManualPop(const Vector3& _position);
    bool    IsExistPopRequest() const { return !popPoints_.empty(); }

    /// <summary>
    /// 次に生成する位置を取得し、内部キューから取り出します。
    /// </summary>
    /// <returns>生成位置。</returns>
    Vector3 GetPopPoint();

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
    void SetPopInterval(float _interval) { popInterval_ = _interval; }
    void SetPopCount(uint32_t _count) { popCount_ = _count; }
    void SetPopRange(const Vector3& _begin, const Vector3& _end)
    {
        popRangeBegin_ = _begin;
        popRangeEnd_ = _end;
    }

    void SetIgnorePosition(const Vector3& _position) { ignorePosition_ = _position; }
    void SetIgnoreRange(float _range) { ignoreRange_ = _range; }
    

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
    TimeMeasurer                   timerOverall_           = {};                   // !< 全体用タイマー
    TimeMeasurer                   timerPop_               = {};                   // !< 生成用タイマー
    TimeMeasurer                   timerPopDelay_          = {};                   // !< 遅延生成用タイマー
    float                   popInterval_            = 1.0f;                 // !< 生成間隔
    std::queue<Vector3>     popPoints_              = {};                   // !< 生成する位置のキュー
    uint32_t                popCount_               = 1;                    // !< 一度に生成する数
    uint32_t                popDelayCount_          = 0;                    // !< 遅延生成する数
    bool                    isEnablePop_            = false;                // !< 生成フラグ


    /// Json
    const std::string       kJsonFileName_          = "PopTimeTable.json";  // !< Jsonファイルパス
    json                    jsonPopTimeTable_       = {};                   // !< Jsonデータ
    PathResolver            pathResolver_           = {};                   // !< ファイルパス検索
    std::vector<PopData>    popData_                = {};                   // !< 生成データ
    size_t                  popDataIndex_           = 0;                    // !< 生成データのインデックス


    /// ランダム生成の範囲
    Vector3                 popRangeBegin_          = {};                   // !< 生成範囲 - 開始
    Vector3                 popRangeEnd_            = {};                   // !< 生成範囲 - 終了


    /// 除外する位置と範囲
    Vector3                 ignorePosition_         = {};                   // !< 除外範囲 - 中心
    float                   ignoreRange_            = 0.0f;                 // !< 除外範囲 - 半径


    /// デバッグ用
    std::unique_ptr<DebugEntry<EnemySpawner>> pDebugEntry_ = {};            // !< デバッグエントリ
    std::unique_ptr<Line>   linesArea_              = {};                   // !< エリアライン
    std::unique_ptr<Line>   linesIgnoreCircle_      = {};                   // !< 禁止エリアライン
    bool                    isDisplayArea_          = false;                // !< 生成範囲表示フラグ

private:
    RandomGenerator*        randomGenerator_        = nullptr;              // !< ランダム生成器
    JSONIO*                 jsonIO_                 = nullptr;              // !< Json入出力
};