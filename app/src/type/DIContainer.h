#pragma once

#include <unordered_map> // unordered_map
#include <typeindex> // type_index

/// <summary>
/// 依存性注入コンテナクラス(WIP)
/// </summary>
class DIContainer
{
public:
    template <typename T>
    /// <summary>
    /// 型Tのインスタンスをコンテナに登録します。
    /// </summary>
    /// <param name="_instance">登録するインスタンスのポインタ。</param>
    void Register(T* _instance)
    {
        container_[std::type_index(typeid(T))] = _instance;
    }

    template <typename T>
    /// <summary>
    /// 型Tのインスタンスを取得します。
    /// 事前に Register されている必要があります。
    /// </summary>
    /// <returns>登録済みインスタンスのポインタ。</returns>
    T* Resolve()
    {
        return static_cast<T*>(container_[std::type_index(typeid(T))]);
    }


private:
    std::unordered_map<std::type_index, void*> container_;
};