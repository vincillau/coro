#include "coro/tcp/conn.hpp"

namespace coro {
namespace tcp {

Promise<size_t> Conn::read(char* buf, size_t size) {
  Promise<size_t> promise;
  socket_.async_receive(boost::asio::buffer(buf, size),
                        [promise](const std::error_code& error, size_t n) {
                          if (error) {
                            promise.reject(error);
                          } else {
                            promise.resolve(n);
                          }
                        });
  return promise;
}

Promise<size_t> Conn::write(const char* buf, size_t size) {
  Promise<size_t> promise;
  socket_.async_send(boost::asio::const_buffer(buf, size),
                     [promise](const std::error_code& error, size_t n) {
                       if (error) {
                         promise.reject(error);
                       } else {
                         promise.resolve(n);
                       }
                     });
  return promise;
}

}  // namespace tcp
}  // namespace coro
