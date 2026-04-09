#pragma once
#include <memory>
#include <drawable/sprite/Sprite.h>
#include <Features/event/EventSubscription.h>
#include <optional>
#include <Features/Input/Input.h>
#include <Features/Event/EventListener.h>

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
        D3D12_GPU_DESCRIPTOR_HANDLE handleGamepad_ = {};
    };
    void Initialize();

    void AddEntry(const Entry& entry)
    {
        entries_.push_back(entry);
    }

private:
    void RegisterSubscriptions();

    std::vector<std::optional<EventSubscription>> subscriptions_;
    std::vector<Entry> entries_;

    // イベントリスナー
    EventListener* pEventListener_ = EventListener::GetInstance();
};