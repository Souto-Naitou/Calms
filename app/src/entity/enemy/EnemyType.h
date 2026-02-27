#pragma once
#include <string>
#include <cassert>

enum class EnemyType
{
    Normal,
    Rusher,
};

static EnemyType EnemyTypeFromString(const std::string& str)
{
    if (str == "Normal") return EnemyType::Normal;
    if (str == "Rusher") return EnemyType::Rusher;
    assert(false && "Invalid EnemyType string");
    return EnemyType::Normal; // デフォルト値
}