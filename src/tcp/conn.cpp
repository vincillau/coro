#include "coro/tcp/conn.hpp"

#include <boost/asio.hpp>

#include "coro/sched/sched.hpp"

namespace coro {
namespace tcp {

Promise<size_t> Socket::read(char* buf, size_t len) {
  Promise<size_t> promise;
  if (len == 0) {
    promise.resolve(0);
    return promise;
  }

  size_t n = Stream::readFromBuf(buf, len);
  if (n > 0) {
    promise.resolve(n);
    return promise;
  }

  socket_.async_receive(boost::asio::mutable_buffer(buf, len),
                        [promise](std::error_code error, size_t n) {
                          if (error) {
                            promise.reject(std::move(error));
                          } else {
                            promise.resolve(n);
                          }
                        });
  return promise;
}

Promise<size_t> Socket::write(const char* buf, size_t len) {
  Promise<size_t> promise;
  socket_.async_send(boost::asio::const_buffer(buf, len),
                     [promise](std::error_code error, size_t n) {
                       if (error) {
                         promise.reject(std::move(error));
                       } else {
                         promise.resolve(n);
                       }
                     });
  return promise;
}

Promise<Conn> connect(const std::string& host, uint16_t port) {
  Promise<Conn> promise;
  auto conn = std::make_shared<Socket>(sched::io_context());
  boost::asio::ip::tcp ::endpoint endpoint(
      boost::asio::ip::address::from_string(host), port);
  conn->socket_.async_connect(endpoint, [conn, promise](std::error_code error) {
    if (error) {
      promise.reject(std::move(error));
    } else {
      promise.resolve(conn);
    }
  });
  return promise;
}

}  // namespace tcp
}  // namespace coro
