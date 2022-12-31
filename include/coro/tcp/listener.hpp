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
  Listener(boost::asio::io_context& io_context,
           const boost::asio::ip::tcp::endpoint& endpoint)
      : acceptor_(io_context, endpoint) {}

  Promise<std::shared_ptr<Conn>> accept();

 private:
  boost::asio::ip::tcp::acceptor acceptor_;
};

Promise<std::shared_ptr<Listener>> listen(const std::string& host,
                                          uint16_t port);

}  // namespace tcp
}  // namespace coro

#endif  // CORO_INCLUDE_CORO_TCP_LISTENER_HPP_
