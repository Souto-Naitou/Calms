#pragma once

#include <drawable/font/NumericView.h>
#include <Features/TimeMeasurer/TimeMeasurer.h>
#include <drawable/sprite/Sprite.h>
#include <array>

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
    void Initialize(bool useSystemClock, double gameDuration);

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
    void SetDisplay(bool isDisplay) { isDisplay_ = isDisplay; }
    void SetNowTime(double time) { nowTime_ = time; }
    bool SetIsEnd(bool isEnd) { isEnd_ = isEnd; }

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

    std::array<D3D12_GPU_DESCRIPTOR_HANDLE, 10> numberTextureHandles_ = {};
    std::unique_ptr<NumericView> pNumericView_ = nullptr;
};