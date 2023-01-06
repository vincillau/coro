#ifndef CORO_INCLUDE_CORO_HTTP_PROTO_REQUEST_HPP_
#define CORO_INCLUDE_CORO_HTTP_PROTO_REQUEST_HPP_

#include <string>
#include <vector>

namespace coro {
namespace http {
namespace proto {

struct Request {
  std::string method;
  std::string url;
  std::string version;
  std::vector<std::pair<std::string, std::string>> headers;
};

}  // namespace proto
}  // namespace http
}  // namespace coro

#endif  // CORO_INCLUDE_CORO_HTTP_PROTO_REQUEST_HPP_
