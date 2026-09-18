#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include <vector>

namespace cocsim::detail {

// Narrow, dependency-free helpers for the checked-in normalized JSON schema.
// They are shared by catalogue, scenario and replay codecs; they are not a
// general-purpose JSON API.
std::string escape_json(const std::string& value);
std::string read_file(const std::string& path, std::string& error);
std::optional<std::string> string_field(const std::string& object, const std::string& key);
std::optional<double> number_field(const std::string& object, const std::string& key);
std::optional<bool> bool_field(const std::string& object, const std::string& key);
std::optional<std::string> array_body(const std::string& document, const std::string& key);
std::vector<std::string> objects_in(const std::string& array);
std::optional<std::string> object_after_key(const std::string& document, const std::string& key);

} // namespace cocsim::detail
