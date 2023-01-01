#ifndef CORO_INCLUDE_CORO_TCP_CONN_HPP_
#define CORO_INCLUDE_CORO_TCP_CONN_HPP_

#include <boost/asio.hpp>

#include "coro/promise.hpp"

namespace coro {
namespace tcp {

class Socket {
 public:
  Socket(boost::asio::io_context& io_context) : socket_(io_context) {}

  Promise<size_t> read(char* buf, size_t size);
  Promise<size_t> write(const char* buf, size_t size);

 private:
  friend class Acceptor;

  boost::asio::ip::tcp::socket socket_;
};

using Conn = std::shared_ptr<Socket>;

}  // namespace tcp
}  // namespace coro

#endif  // CORO_INCLUDE_CORO_TCP_CONN_HPP_
