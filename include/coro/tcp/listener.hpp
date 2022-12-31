#ifndef CORO_INCLUDE_CORO_TCP_LISTENER_HPP_
#define CORO_INCLUDE_CORO_TCP_LISTENER_HPP_

#include <boost/asio.hpp>
#include <memory>
#include <string>
#include <system_error>

#include "conn.hpp"
#include "coro/promise.hpp"

namespace coro {
namespace tcp {

class Listener {
 public:
  Listener(boost::asio::ip::tcp::acceptor acceptor)
      : acceptor_(std::move(acceptor)) {}

  Promise<std::shared_ptr<Conn>> accept();

 private:
  boost::asio::ip::tcp::acceptor acceptor_;
};

std::shared_ptr<Listener> listen(const std::string& host, uint16_t port,
                                 std::error_code* error);
std::shared_ptr<Listener> listen(const std::string& host, uint16_t port);
inline static std::shared_ptr<Listener> listen(uint16_t port,
                                               std::error_code* error) {
  return listen("127.0.0.1", port, error);
}
inline static std::shared_ptr<Listener> listen(uint16_t port) {
  return listen("127.0.0.1", port);
}

}  // namespace tcp
}  // namespace coro

#endif  // CORO_INCLUDE_CORO_TCP_LISTENER_HPP_
