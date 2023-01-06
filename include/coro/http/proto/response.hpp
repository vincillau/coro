#ifndef CORO_INCLUDE_CORO_HTTP_PROTO_RESPONSE_HPP_
#define CORO_INCLUDE_CORO_HTTP_PROTO_RESPONSE_HPP_

#include <string>
#include <vector>

namespace coro {
namespace http {
namespace proto {

struct Response {
  std::string version;
  int code;
  std::string reason;
  std::vector<std::pair<std::string, std::string>> headers;
};

}  // namespace proto
}  // namespace http
}  // namespace coro

#endif  // CORO_INCLUDE_CORO_HTTP_PROTO_RESPONSE_HPP_
