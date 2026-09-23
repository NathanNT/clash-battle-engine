#include "cocsim/core.hpp"
namespace cocsim {
const GameData& GameData::empty() { static const GameData data; return data; }
} // namespace cocsim
