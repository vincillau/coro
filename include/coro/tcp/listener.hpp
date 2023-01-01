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

class Acceptor {
 public:
  Acceptor(boost::asio::ip::tcp::acceptor acceptor)
      : acceptor_(std::move(acceptor)) {}

  Promise<Conn> accept();

 private:
  boost::asio::ip::tcp::acceptor acceptor_;
};

using Listener = std::shared_ptr<Acceptor>;

Listener listen(const std::string& host, uint16_t port, std::error_code* error);
Listener listen(const std::string& host, uint16_t port);
inline static Listener listen(uint16_t port, std::error_code* error) {
  return listen("127.0.0.1", port, error);
}
inline static Listener listen(uint16_t port) {
  return listen("127.0.0.1", port);
}

}  // namespace tcp
}  // namespace coro

#endif  // CORO_INCLUDE_CORO_TCP_LISTENER_HPP_
