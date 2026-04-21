#pragma once
#include <drawable/sprite/Sprite.h>
#include <Features/event/EventSubscription.h>
#include <optional>
#include <Features/Event/EventListener.h>
#include <Vector2.h>

/// <summary>
/// 入力デバイスに応じて表示を切り替えるスプライトクラス。
/// </summary>
class InputAwareSprite
{
public:
    struct Entry
    {
        Sprite* pSprite_ = nullptr;
        D3D12_GPU_DESCRIPTOR_HANDLE handleKeyboard_ = {};
        std::optional<Vector2> sizeKeyboard_ = std::nullopt;
        std::optional<Vector2> positionKeyboard_ = std::nullopt;
        D3D12_GPU_DESCRIPTOR_HANDLE handleGamepad_ = {};
        std::optional<Vector2> sizeGamepad_ = std::nullopt;
        std::optional<Vector2> positionGamepad_ = std::nullopt;
    };
    void Initialize();

    void AddEntry(const Entry& entry)
    {
        entries_.push_back(entry);
    }

    /// <summary>
    /// 現在の入力モードをテクスチャに適用します。
    /// </summary>
    void ApplyCurrentMode();

private:
    void RegisterSubscriptions();
    void ApplyToGamepad();
    void ApplyToKeyboard();

    std::vector<std::optional<EventSubscription>> subscriptions_;
    std::vector<Entry> entries_;

    // イベントリスナー
    EventListener* pEventListener_ = EventListener::GetInstance();
};