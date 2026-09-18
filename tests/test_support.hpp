#pragma once

#include <cstdlib>
#include <iostream>

namespace cocsim::test {

inline void require(bool condition, const char* expression, const char* file, int line) {
  if (condition) return;
  std::cerr << "requirement failed: " << expression << " @ " << file << ':' << line << '\n';
  std::exit(EXIT_FAILURE);
}

} // namespace cocsim::test

#define COCSIM_REQUIRE(expression) \
  ::cocsim::test::require(static_cast<bool>(expression), #expression, __FILE__, __LINE__)
