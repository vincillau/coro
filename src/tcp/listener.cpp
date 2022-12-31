#include "coro/tcp/listener.hpp"

#include "coro/exception.hpp"
#include "coro/sched/sched.hpp"

namespace coro {
namespace tcp {

Promise<std::shared_ptr<Conn>> Listener::accept() {
  Promise<std::shared_ptr<Conn>> promise;
  auto conn = std::make_shared<Conn>(sched::Sched::io_context());
  acceptor_.async_accept(conn->socket_,
                         [conn, promise](const std::error_code& error) {
                           if (error) {
                             promise.reject(error);
                           } else {
                             promise.resolve(conn);
                           }
                         });
  return promise;
}

Promise<std::shared_ptr<Listener>> listen(const std::string& host,
                                          uint16_t port) {
  Promise<std::shared_ptr<Listener>> promise;
  boost::asio::ip::tcp ::endpoint endpoint(
      boost::asio::ip::address::from_string(host), port);
  auto listener =
      std::make_shared<Listener>(sched::Sched::io_context(), endpoint);
  promise.resolve(listener);
  return promise;
}

}  // namespace tcp
}  // namespace coro
