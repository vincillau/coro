#ifndef CORO_INCLUDE_CORO_HTTP_PROTO_PARSE_HPP_
#define CORO_INCLUDE_CORO_HTTP_PROTO_PARSE_HPP_

#include <string>

namespace coro {
namespace http {
namespace proto {

bool parseReqStartLine(const char* line, std::string& method, std::string& url,
                       std::string& version);

bool parseRespStartLine(const char* line, std::string& version, int& code,
                        std::string& reason);

bool parseHeader(const char* line, std::string& name, std::string& value);

}  // namespace proto
}  // namespace http
}  // namespace coro

#endif  // CORO_INCLUDE_CORO_HTTP_PROTO_PARSE_HPP_
