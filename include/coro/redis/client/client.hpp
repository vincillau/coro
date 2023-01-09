#ifndef CORO_INCLUDE_CORO_REDIS_CLIENT_CLIENT_HPP_
#define CORO_INCLUDE_CORO_REDIS_CLIENT_CLIENT_HPP_

#include <cstdint>
#include <string>
#include <vector>

#include "coro/promise.hpp"
#include "coro/redis/protocol/protocol.hpp"
#include "coro/tcp.hpp"

namespace coro {
namespace redis {
namespace client {

class Client {
 public:
  using Request = std::vector<std::string>;
  using Response = std::shared_ptr<protocol::Field>;

  Client() = default;
  ~Client() { close(); }

  Promise<void> connect(const std::string& host, uint16_t port);
  Promise<Response> exec(Request req);
  void close();

 private:
  tcp::Conn conn_;
};

}  // namespace client
}  // namespace redis
}  // namespace coro

#endif  // CORO_INCLUDE_CORO_REDIS_CLIENT_CLIENT_HPP_
