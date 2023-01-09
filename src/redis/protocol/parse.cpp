#include "coro/redis/protocol/parse.hpp"

#include <cstring>

namespace coro {
namespace redis {
namespace protocol {

std::shared_ptr<SimpleStringField> parseSimpleString(const char* line,
                                                     size_t n) {
  std::string value(line + 1, n - 3);
  return SimpleStringField::from(std::move(value));
}

std::shared_ptr<ErrorField> parseError(const char* line, size_t n) {
  std::string value(line + 1, n - 3);
  return ErrorField::from(std::move(value));
}

std::shared_ptr<IntegerField> parseInteger(const char* line, size_t n) {
  uint64_t u64 = 0;
  bool positive = true;
  const char* begin = line + 1;
  const char* end = line + n - 2;
  if (*begin == '-') {
    positive = false;
    begin++;
  }
  if (begin == end) {
    return nullptr;
  }
  for (const char* iter = begin; iter < end; iter++) {
    if (*iter < '0' || *iter > '9') {
      return nullptr;
    }
    uint64_t digit = *iter - '0';
    if (u64 > (1ULL << 63) / 10) {
      return nullptr;
    }
    u64 *= 10;
    if (u64 > (1ULL << 63) - digit) {
      u64 += digit;
    }
  }
  if (u64 == (1ULL << 63)) {
    if (!positive) {
      return IntegerField::from(INT64_MIN);
    }
    return nullptr;
  }

  int64_t value = 0;
  if (positive) {
    value = static_cast<int64_t>(u64);
  } else {
    value = -static_cast<int64_t>(u64);
  }
  return IntegerField::from(value);
}

int64_t parseBulkStringLength(const char* line, size_t n) {
  if (n == (sizeof("$-1\r\n") - 1)) {
    if (memcmp(line, "$-1\r\n", n) == 0) {
      return -1;
    }
  }

  const char* begin = line + 1;
  const char* end = line + n - 2;
  // Redis 中字符串的最大长度为 512M，因此长度部分的长度不超过 9。
  if (begin == end || end - begin > 9) {
    return -2;
  }

  int64_t len = 0;
  for (const char* iter = begin; iter < end; iter++) {
    if (*iter < '0' || *iter > '9') {
      return -2;
    }
    int64_t digit = *iter - '0';
    len *= 10;
    len += digit;
  }

  if (len > 512 * 1024 * 1024) {
    return -2;
  }

  return len;
}

int64_t parseArrayLength(const char* line, size_t n) {
  if (n == (sizeof("*-1\r\n") - 1)) {
    if (memcmp(line, "*-1\r\n", n) == 0) {
      return -1;
    }
  }

  const char* begin = line + 1;
  const char* end = line + n - 2;
  // Redis 中数组元素数目最大为 2^32-1，因此长度部分的长度不超过 10。
  if (begin == end || end - begin > 9) {
    return -2;
  }

  int64_t len = 0;
  for (const char* iter = begin; iter < end; iter++) {
    if (*iter < '0' || *iter > '9') {
      return -2;
    }
    int64_t digit = *iter - '0';
    len *= 10;
    len += digit;
  }

  if (len > UINT32_MAX) {
    return -2;
  }

  return len;
}

}  // namespace protocol
}  // namespace redis
}  // namespace coro
