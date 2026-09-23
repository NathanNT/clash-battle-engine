#pragma once
#include <cstdint>
#include <initializer_list>
#include <map>
#include <string>
#include <vector>
namespace clash_battle_engine::json {
struct Value {
  enum class Type { Null, Boolean, Number, String, Array, Object } type{Type::Null};
  std::string scalar;
  std::vector<Value> array;
  std::map<std::string, Value> object;
  const Value& at(const std::string& key) const;
  void keys(std::initializer_list<const char*> expected) const;
  std::string string() const;
  std::int64_t integer() const;
  std::uint64_t unsigned_integer() const;
};
Value parse(const std::string& text);
std::string stringify(const Value& value);
} // namespace clash_battle_engine::json
