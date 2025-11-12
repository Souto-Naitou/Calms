#pragma once

#include <Features/TimeMeasurer/TimeMeasurer.h>
#include <drawable/sprite/Sprite.h>
#include <array>
#include <Features/Layer/Canvas.h>

/// <summary>
/// ゲーム内タイマークラス
/// </summary>
class InGameTimer
{
public:
    /// <summary>
    /// タイマーを初期化します。
    /// システムクロックの使用有無とゲーム全体の制限時間を設定します。
    /// </summary>
    /// <param name="_useSystemClock">システムクロックを使用する場合は true。</param>
    /// <param name="_gameDuration">ゲームの制限時間（秒）。</param>
    void Initialize(bool _useSystemClock, double _gameDuration);

    /// <summary>
    /// タイマーの状態を更新します。
    /// 表示や終了判定に必要な内部値を更新します。
    /// </summary>
    void Update();

    /// <summary>
    /// タイマーの描画を行います。
    /// </summary>
    void Draw1F();

    /// <summary>
    /// タイマーの終了処理を行います。
    /// </summary>
    void Finalize();

    /// <summary>
    /// タイマーを開始します。
    /// </summary>
    void Start();

    /// <summary>
    /// タイマーをリセットします。
    /// </summary>
    void Reset();

public: /// Getter
    bool IsEnd() const { return isEnd_; }
    bool IsNextScene() const { return isNextScene_; }
    double GetNowTime() const { return nowTime_; }



public: /// Setter
    void SetDisplay(bool _isDisplay) { isDisplay_ = _isDisplay; }
    void SetNowTime(double _time) { nowTime_ = _time; }
    bool SetIsEnd(bool _isEnd) { isEnd_ = _isEnd; }

private:
    /// <summary>
    /// 現在時刻・残り時間を更新します。
    /// </summary>
    void CurrentTimeUpdate();

    /// <summary>
    /// 表示用スプライトのインデックスや不透明度など視覚要素を更新します。
    /// </summary>
    void SpriteUpdate();

private:
    /// タイマー
    std::unique_ptr<TimeMeasurer> pTimer_ = nullptr;
    double nowTime_ = 0.0;

    bool isStart_ = false;
    bool isDisplay_ = false;
    bool isUseSystemClock_ = false;

    double gameDuration_ = 0.0;
    const double changeInterval_ = 2.0;

    bool isEnd_ = false;
    bool isNextScene_ = false;

    std::array<std::unique_ptr<Sprite>, 10> tensPlaceNums_ = {};
    std::array<std::unique_ptr<Sprite>, 10> onesPlaceNums_ = {};

    // 表示するスプライトのインデックス
    uint32_t indexTensPlace_ = 0;
    uint32_t indexOnesPlace_ = 0;
};