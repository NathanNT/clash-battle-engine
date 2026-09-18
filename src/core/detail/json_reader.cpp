#include "json_reader.hpp"

#include <cctype>
#include <cstdlib>
#include <fstream>
#include <iterator>

namespace cocsim::detail {
namespace {

std::optional<std::size_t> field_value_start(const std::string& object,
                                             const std::string& key,
                                             std::size_t search_from = 0) {
  const std::string needle = "\"" + key + "\"";
  while (search_from < object.size()) {
    const auto found = object.find(needle, search_from);
    if (found == std::string::npos) break;
    auto cursor = found + needle.size();
    while (cursor < object.size() && std::isspace(static_cast<unsigned char>(object[cursor]))) ++cursor;
    if (cursor < object.size() && object[cursor] == ':') {
      ++cursor;
      while (cursor < object.size() && std::isspace(static_cast<unsigned char>(object[cursor]))) ++cursor;
      return cursor;
    }
    search_from = found + needle.size();
  }
  return std::nullopt;
}

std::optional<std::size_t> matching_json_delimiter(const std::string& text,
                                                    std::size_t start,
                                                    char open,
                                                    char close) {
  if (start >= text.size() || text[start] != open) return std::nullopt;
  int depth = 0;
  bool in_string = false;
  bool escaped = false;
  for (std::size_t index = start; index < text.size(); ++index) {
    const char value = text[index];
    if (in_string) {
      if (escaped) escaped = false;
      else if (value == '\\') escaped = true;
      else if (value == '"') in_string = false;
      continue;
    }
    if (value == '"') { in_string = true; continue; }
    if (value == open) ++depth;
    else if (value == close && --depth == 0) return index;
  }
  return std::nullopt;
}

} // namespace

std::string escape_json(const std::string& value) {
  std::string escaped;
  for (const char character : value) {
    if (character == '"' || character == '\\') escaped += '\\';
    escaped += character;
  }
  return escaped;
}

std::string read_file(const std::string& path, std::string& error) {
  std::ifstream stream(path);
  if (!stream) {
    error = "cannot open " + path;
    return {};
  }
  return {std::istreambuf_iterator<char>(stream), {}};
}

std::optional<std::string> string_field(const std::string& object, const std::string& key) {
  std::size_t search_from = 0;
  for (;;) {
    const auto start = field_value_start(object, key, search_from);
    if (!start) break;
    search_from = *start + 1;
    if (*start >= object.size() || object[*start] != '"') continue;
    std::string value;
    bool escaped = false;
    for (std::size_t cursor = *start + 1; cursor < object.size(); ++cursor) {
      const char character = object[cursor];
      if (escaped) {
        switch (character) {
          case 'n': value += '\n'; break;
          case 'r': value += '\r'; break;
          case 't': value += '\t'; break;
          default: value += character; break;
        }
        escaped = false;
      } else if (character == '\\') escaped = true;
      else if (character == '"') return value;
      else value += character;
    }
  }
  return std::nullopt;
}

std::optional<double> number_field(const std::string& object, const std::string& key) {
  std::size_t search_from = 0;
  for (;;) {
    const auto start = field_value_start(object, key, search_from);
    if (!start) break;
    search_from = *start + 1;
    char* end = nullptr;
    const double value = std::strtod(object.c_str() + *start, &end);
    if (end != object.c_str() + *start) return value;
  }
  return std::nullopt;
}

std::optional<bool> bool_field(const std::string& object, const std::string& key) {
  std::size_t search_from = 0;
  for (;;) {
    const auto start = field_value_start(object, key, search_from);
    if (!start) break;
    search_from = *start + 1;
    if (object.compare(*start, 4, "true") == 0) return true;
    if (object.compare(*start, 5, "false") == 0) return false;
  }
  return std::nullopt;
}

std::optional<std::string> array_body(const std::string& document, const std::string& key) {
  std::size_t search_from = 0;
  for (;;) {
    const auto start = field_value_start(document, key, search_from);
    if (!start) return std::nullopt;
    search_from = *start + 1;
    if (*start >= document.size() || document[*start] != '[') continue;
    const auto end = matching_json_delimiter(document, *start, '[', ']');
    return end ? std::optional<std::string>(document.substr(*start + 1, *end - *start - 1))
               : std::nullopt;
  }
}

std::vector<std::string> objects_in(const std::string& array) {
  std::vector<std::string> result;
  int depth = 0;
  std::size_t start = 0;
  bool in_string = false;
  bool escaped = false;
  for (std::size_t index = 0; index < array.size(); ++index) {
    const char value = array[index];
    if (in_string) {
      if (escaped) escaped = false;
      else if (value == '\\') escaped = true;
      else if (value == '"') in_string = false;
      continue;
    }
    if (value == '"') { in_string = true; continue; }
    if (value == '{' && depth++ == 0) start = index;
    else if (value == '}' && depth > 0 && --depth == 0) result.push_back(array.substr(start, index - start + 1));
  }
  return result;
}

std::optional<std::string> object_after_key(const std::string& document, const std::string& key) {
  const auto start = field_value_start(document, key);
  if (!start || *start >= document.size() || document[*start] != '{') return std::nullopt;
  const auto end = matching_json_delimiter(document, *start, '{', '}');
  return end ? std::optional<std::string>(document.substr(*start, *end - *start + 1))
             : std::nullopt;
}

} // namespace cocsim::detail
