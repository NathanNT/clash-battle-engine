#include "cocsim/core.hpp"
namespace cocsim {
const GameData& GameData::baseline() { static const GameData data; return data; }
} // namespace cocsim
