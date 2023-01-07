#ifndef CORO_INCLUDE_CORO_HTTP_PROTOCOL_REQUEST_HPP_
#define CORO_INCLUDE_CORO_HTTP_PROTOCOL_REQUEST_HPP_

#include <string>
#include <vector>

namespace coro {
namespace http {
namespace protocol {

struct Request {
  std::string method;
  std::string url;
  std::string version;
  std::vector<std::pair<std::string, std::string>> headers;
};

}  // namespace protocol
}  // namespace http
}  // namespace coro

#endif  // CORO_INCLUDE_CORO_HTTP_PROTOCOL_REQUEST_HPP_
