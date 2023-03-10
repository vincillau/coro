#include "coro/tcp/listener.hpp"

#include "coro/exception.hpp"
#include "coro/sched/sched.hpp"

namespace coro {
namespace tcp {
namespace impl {

Promise<std::shared_ptr<Conn>> Listener::accept() {
  Promise<std::shared_ptr<Conn>> promise;
  auto conn = std::make_shared<Conn>(sched::io_context());
  acceptor_.async_accept(conn->socket_, [conn, promise](std::error_code error) {
    if (error) {
      promise.reject(std::move(error));
    } else {
      promise.resolve(conn);
    }
  });
  return promise;
}

std::shared_ptr<Listener> listen(const std::string& host, uint16_t port,
                                 std::error_code* error) {
  boost::asio::ip::tcp ::endpoint endpoint(
      boost::asio::ip::address::from_string(host), port);
  boost::asio::ip::tcp::acceptor acceptor(sched::io_context());
  boost::system::error_code err;

  acceptor.open(endpoint.protocol(), err);
  if (err) {
    if (error) {
      *error = err;
    }
    return nullptr;
  }

  boost::asio::socket_base::reuse_address option(true);
  acceptor.set_option(option, err);
  if (err) {
    if (error) {
      *error = err;
    }
    return nullptr;
  }

  acceptor.bind(endpoint, err);
  if (err) {
    if (error) {
      *error = err;
    }
    return nullptr;
  }

  acceptor.listen(boost::asio::socket_base::max_listen_connections, err);
  if (err) {
    if (error) {
      *error = err;
    }
    return nullptr;
  }

  auto listener = std::make_shared<Listener>(std::move(acceptor));
  return listener;
}

std::shared_ptr<Listener> listen(const std::string& host, uint16_t port) {
  std::error_code error;
  auto listener = listen(host, port, &error);
  if (error) {
    throw Exception(error);
  }
  return listener;
}

}  // namespace impl
}  // namespace tcp
}  // namespace coro
