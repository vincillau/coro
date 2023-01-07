#include "coro/http/protocol/parse.hpp"

#include <cctype>
#include <cstring>

namespace coro {
namespace http {
namespace protocol {

bool parseReqStartLine(const char* line, std::string& method, std::string& url,
                       std::string& version) {
  const char* cursor = line;
  method.clear();
  url.clear();
  version.clear();

  for (; !isblank(*cursor); cursor++) {
    if (*cursor == '\n') {
      return false;
    }
    method.push_back(*cursor);
  }
  if (method.empty()) {
    return false;
  }
  cursor++;
  for (; isblank(*cursor); cursor++) {
    if (*cursor == '\n') {
      return false;
    }
  }

  for (; !isblank(*cursor); cursor++) {
    if (*cursor == '\n') {
      return false;
    }
    url.push_back(*cursor);
  }
  if (url.empty()) {
    return false;
  }
  cursor++;
  for (; isblank(*cursor); cursor++) {
    if (*cursor == '\n') {
      return false;
    }
  }

  for (; *cursor != '\r'; cursor++) {
    if (*cursor == '\n') {
      return false;
    }
    version.push_back(*cursor);
  }
  if (version.empty()) {
    return false;
  }
  cursor++;
  return *cursor == '\n';
}

bool parseRespStartLine(const char* line, std::string& version, int& code,
                        std::string& reason) {
  const char* cursor = line;
  version.clear();
  code = 0;
  reason.clear();

  for (; !isblank(*cursor); cursor++) {
    if (*cursor == '\n') {
      return false;
    }
    version.push_back(*cursor);
  }
  if (version.empty()) {
    return false;
  }
  cursor++;
  for (; isblank(*cursor); cursor++) {
    if (*cursor == '\n') {
      return false;
    }
  }

  std::string code_str;
  for (; !isblank(*cursor); cursor++) {
    if (*cursor == '\n') {
      return false;
    }
    code_str.push_back(*cursor);
  }
  if (code_str.length() != 3) {
    return false;
  }
  for (char chr : code_str) {
    if (chr < '0' || chr > '9') {
      return false;
    }
  }
  int digit1 = code_str[0] - '0';
  int digit2 = code_str[1] - '0';
  int digit3 = code_str[2] - '0';
  code = digit1 * 100 + digit2 * 10 + digit3;
  cursor++;
  for (; isblank(*cursor); cursor++) {
    if (*cursor == '\n') {
      return false;
    }
  }

  for (; *cursor != '\r'; cursor++) {
    if (*cursor == '\n') {
      return false;
    }
    reason.push_back(*cursor);
  }
  if (reason.empty()) {
    return false;
  }
  cursor++;
  return *cursor == '\n';
}

bool parseHeader(const char* line, std::string& name, std::string& value) {
  const char* cursor = line;
  name.clear();
  value.clear();

  for (; *cursor != ':'; cursor++) {
    if (*cursor == '\n') {
      return false;
    }
    name.push_back(*cursor);
  }
  if (name.empty()) {
    return false;
  }
  cursor++;
  for (; isblank(*cursor); cursor++) {
    if (*cursor == '\n') {
      return false;
    }
  }

  for (; *cursor != '\r'; cursor++) {
    if (*cursor == '\n') {
      return false;
    }
    value.push_back(*cursor);
  }
  if (value.empty()) {
    return false;
  }
  cursor++;
  return *cursor == '\n';
}

}  // namespace protocol
}  // namespace http
}  // namespace coro
