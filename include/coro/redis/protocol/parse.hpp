#ifndef CORO_INCLUDE_CORO_REDIS_PROTOCOL_PARSE_HPP_
#define CORO_INCLUDE_CORO_REDIS_PROTOCOL_PARSE_HPP_

#include "field.hpp"

namespace coro {
namespace redis {
namespace protocol {

std::shared_ptr<SimpleStringField> parseSimpleString(const char* line,
                                                     size_t n);
std::shared_ptr<ErrorField> parseError(const char* line, size_t n);
std::shared_ptr<IntegerField> parseInteger(const char* line, size_t n);
int64_t parseBulkStringLength(const char* line, size_t n);
int64_t parseArrayLength(const char* line, size_t n);

}  // namespace protocol
}  // namespace redis
}  // namespace coro

#endif  // CORO_INCLUDE_CORO_REDIS_PROTOCOL_PARSE_HPP_
