#pragma once

#include <Features/RandomGenerator/RandomGenerator.h>
#include <Timer/Timer.h>
#include <Vector3.h>
#include <Features/GameEye/GameEye.h>
#include <Features/Line/Line.h>
#include <JsonParser/JsonLoader.h>
#include <JsonParser/json.h>
#include <Utility/FilePathSearcher/FilePathSearcher.h>
#include <queue>
#include <array>
#include <string>

class EnemyPopSystem
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
    void Initialize();
    void Finalize();
    void Update();
    void DrawArea();

    void ManualPop();
    void ManualPop(const Vector3& _position);

    bool IsExistPopRequest() const { return !popPoints_.empty(); }
    Vector3 GetPopPoint();

    void StartPop();
    void StopPop();

    bool IsEnablePop() const { return isEnablePop_; }

public: /// Setter
    void SetPopInterval(float _interval) { popInterval_ = _interval; }
    void SetPopCount(uint32_t _count) { popCount_ = _count; }
    void SetGameEye(GameEye* _eye) { ModifyGameEye(_eye); }

    void SetPopRange(const Vector3& _begin, const Vector3& _end)
    {
        popRangeBegin_ = _begin;
        popRangeEnd_ = _end;
    }

    void SetIgnorePosition(const Vector3& _position) { ignorePosition_ = _position; }
    void SetIgnoreRange(float _range) { ignoreRange_ = _range; }
    

private:
    void PopRandom();  // ランダム生成
    void DebugWindow();
    void ModifyGameEye(GameEye* _eye);
    void InitPopData();
    void UpdatePop();


private:
    Timer                   timerOverall_           = {};                   // !< 全体用タイマー
    Timer                   timerPop_               = {};                   // !< 生成用タイマー
    float                   popInterval_            = 1.0f;                 // !< 生成間隔
    std::queue<Vector3>     popPoints_              = {};                   // !< 生成する位置のキュー
    uint32_t                popCount_               = 1;                    // !< 一度に生成する数
    bool                    isEnablePop_            = false;                // !< 生成フラグ


    /// Json
    const std::string       kJsonFileName_          = "PopTimeTable.json";  // !< Jsonファイルパス
    Json::Value             jsonPopTimeTable_       = {};                   // !< Jsonデータ
    FilePathSearcher        filePathSearcher_       = {};                   // !< ファイルパス検索
    std::vector<PopData>    popData_                = {};                   // !< 生成データ
    size_t                  popDataIndex_           = 0;                    // !< 生成データのインデックス


    /// ランダム生成の範囲
    Vector3                 popRangeBegin_          = {};                   // !< 生成範囲 - 開始
    Vector3                 popRangeEnd_            = {};                   // !< 生成範囲 - 終了


    /// 除外する位置と範囲
    Vector3                 ignorePosition_         = {};                   // !< 除外範囲 - 中心
    float                   ignoreRange_            = 0.0f;                 // !< 除外範囲 - 半径


    /// デバッグ用
    Line*                   linesArea_              = {};                   // !< エリアライン
    Line*                   linesIgnoreCircle_      = {};                   // !< 禁止エリアライン
    std::string             name_                   = "EnemyPopSystem";     // !< 名前
    bool                    isDisplayArea_          = false;                // !< 生成範囲表示フラグ

private:
    RandomGenerator*        randomGenerator_        = nullptr;              // !< ランダム生成器
    JsonLoader&             jsonLoader_             = JsonLoader::GetInstance(); // !< Jsonローダー
};