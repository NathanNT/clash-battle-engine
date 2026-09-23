#include "clash_battle_engine/core.hpp"
namespace clash_battle_engine {
const GameData& GameData::baseline() { static const GameData data; return data; }
} // namespace clash_battle_engine
