#pragma once
#include <string>
#include <cassert>
#include <array>

enum class EnemyType
{
    Normal,
    Rusher,
};

struct EnemyTypeEntry
{
    EnemyType type;
    std::string name;
};

inline static const std::array kEnemyTypeTable =
{
    EnemyTypeEntry(EnemyType::Normal, "Normal"),
    EnemyTypeEntry(EnemyType::Rusher, "Rusher")
};

inline EnemyType EnemyTypeFromString(const std::string& str)
{
    for (const auto& entry : kEnemyTypeTable)
    {
        if (entry.name == str)
        {
            return entry.type;
        }
    }
    assert(false && "Invalid EnemyType string");
    return EnemyType::Normal; // デフォルト値
}

inline std::string EnemyTypeToString(EnemyType type)
{
    for (const auto& entry : kEnemyTypeTable)
    {
        if (entry.type == type)
        {
            return entry.name;
        }
    }
    assert(false && "Invalid EnemyType");
    return ""; // デフォルト値
}