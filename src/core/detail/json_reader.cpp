#include "json_reader.hpp"
#include <charconv>
#include <cctype>
#include <cstdint>
#include <initializer_list>
#include <stdexcept>
#include <string_view>
namespace cocsim::json {
const Value& Value::at(const std::string& key) const {
  if (type != Type::Object) throw std::runtime_error("expected JSON object");
  auto it = object.find(key);
  if (it == object.end()) throw std::runtime_error("missing field: " + key);
  return it->second;
}
void Value::keys(std::initializer_list<const char*> expected) const {
  if (type != Type::Object || object.size() != expected.size()) throw std::runtime_error("unexpected JSON fields");
  for (const char* key : expected) (void)at(key);
}
std::string Value::string() const {
  if (type != Type::String) throw std::runtime_error("expected JSON string");
  return scalar;
}
std::int64_t Value::integer() const {
  if (type != Type::Number || scalar.find_first_of(".eE") != std::string::npos) throw std::runtime_error("expected integer");
  std::int64_t result{};
  auto [end, error] = std::from_chars(scalar.data(), scalar.data() + scalar.size(), result);
  if (error != std::errc{} || end != scalar.data() + scalar.size()) throw std::runtime_error("integer out of range");
  return result;
}
std::uint64_t Value::unsigned_integer() const {
  if (type != Type::Number || scalar.empty() || scalar[0] == '-' || scalar.find_first_of(".eE") != std::string::npos) throw std::runtime_error("expected unsigned integer");
  std::uint64_t result{};
  auto [end, error] = std::from_chars(scalar.data(), scalar.data() + scalar.size(), result);
  if (error != std::errc{} || end != scalar.data() + scalar.size()) throw std::runtime_error("integer out of range");
  return result;
}
class Parser {
 public:
  explicit Parser(std::string_view text) : text_(text) {}
  Value run() {
    auto value = read();
    space();
    if (pos_ != text_.size()) fail();
    return value;
  }
 private:
  std::string_view text_;
  std::size_t pos_{};
  [[noreturn]] void fail() const { throw std::runtime_error("invalid JSON at byte " + std::to_string(pos_)); }
  void space() { while (pos_ < text_.size() && std::isspace(static_cast<unsigned char>(text_[pos_]))) ++pos_; }
  bool take(char ch) { space(); if (pos_ < text_.size() && text_[pos_] == ch) { ++pos_; return true; } return false; }
  std::string quoted() {
    if (!take('"')) fail();
    std::string out;
    while (pos_ < text_.size()) {
      char ch = text_[pos_++];
      if (ch == '"') return out;
      if (static_cast<unsigned char>(ch) < 0x20) fail();
      if (ch == '\\') {
        if (pos_ == text_.size()) fail();
        ch = text_[pos_++];
        switch (ch) {
          case '"': case '\\': case '/': out += ch; break;
          case 'b': out += '\b'; break; case 'f': out += '\f'; break;
          case 'n': out += '\n'; break; case 'r': out += '\r'; break; case 't': out += '\t'; break;
          default: fail(); // No escaped Unicode is needed for this ASCII schema.
        }
      } else out += ch;
    }
    fail();
  }
  Value read() {
    space();
    if (pos_ == text_.size()) fail();
    Value value;
    if (text_[pos_] == '{') {
      ++pos_; value.type = Value::Type::Object;
      if (take('}')) return value;
      do {
        const auto key = quoted();
        if (!take(':')) fail();
        if (!value.object.emplace(key, read()).second) fail();
        if (take('}')) return value;
      } while (take(','));
      fail();
    }
    if (text_[pos_] == '[') {
      ++pos_; value.type = Value::Type::Array;
      if (take(']')) return value;
      do { value.array.push_back(read()); if (take(']')) return value; } while (take(','));
      fail();
    }
    if (text_[pos_] == '"') { value.type = Value::Type::String; value.scalar = quoted(); return value; }
    const auto start = pos_;
    if (text_[pos_] == '-') ++pos_;
    if (pos_ < text_.size() && std::isdigit(static_cast<unsigned char>(text_[pos_]))) {
      if (text_[pos_] == '0') ++pos_;
      else while (pos_ < text_.size() && std::isdigit(static_cast<unsigned char>(text_[pos_]))) ++pos_;
      if (pos_ < text_.size() && text_[pos_] == '.') { ++pos_; while (pos_ < text_.size() && std::isdigit(static_cast<unsigned char>(text_[pos_]))) ++pos_; }
      if (pos_ < text_.size() && (text_[pos_] == 'e' || text_[pos_] == 'E')) {
        ++pos_; if (pos_ < text_.size() && (text_[pos_] == '+' || text_[pos_] == '-')) ++pos_;
        while (pos_ < text_.size() && std::isdigit(static_cast<unsigned char>(text_[pos_]))) ++pos_;
      }
      value.type = Value::Type::Number; value.scalar = std::string(text_.substr(start, pos_ - start));
      return value;
    }
    fail();
  }
};
Value parse(const std::string& text) { return Parser(text).run(); }
std::string stringify(const Value& value) {
  switch (value.type) {
    case Value::Type::Number: return value.scalar;
    case Value::Type::String: {
      std::string out="\"";
      for (char ch:value.scalar) {
        if (ch=='"' || ch=='\\') { out+='\\'; out+=ch; }
        else if (ch=='\n') out+="\\n";
        else if (ch=='\r') out+="\\r";
        else if (ch=='\t') out+="\\t";
        else out+=ch;
      }
      return out+"\"";
    }
    case Value::Type::Array: {
      std::string out="[";
      for (const auto& child:value.array) { if(out.size()>1) out+=","; out+=stringify(child); }
      return out+"]";
    }
    case Value::Type::Object: {
      std::string out="{";
      for (const auto& [key,child]:value.object) {
        if(out.size()>1) out+=",";
        Value name; name.type=Value::Type::String; name.scalar=key;
        out+=stringify(name)+":"+stringify(child);
      }
      return out+"}";
    }
    default: throw std::runtime_error("unsupported JSON value");
  }
}
} // namespace cocsim::json
