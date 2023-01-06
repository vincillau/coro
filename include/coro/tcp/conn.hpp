#ifndef CORO_INCLUDE_CORO_TCP_CONN_HPP_
#define CORO_INCLUDE_CORO_TCP_CONN_HPP_

#include <boost/asio.hpp>

#include "coro/promise.hpp"
#include "coro/stream.hpp"

namespace coro {
namespace tcp {

/**
 * @brief 表示 TCP 连接的类。
 */
class Socket : public Stream {
 public:
  Socket(boost::asio::io_context& io_context) : socket_(io_context) {}

  /**
   * @brief 读取最多 len 个字节。
   * @param buf 缓冲区。
   * @param len 缓冲区大小。
   * @return Promise<size_t> 读取到的字节数，可能小于 len。
   */
  Promise<size_t> read(char* buf, size_t len) override;

  /**
   * @brief 向流中写入 len 个字节。
   * @param buf 缓冲区。
   * @param len 缓冲区大小。
   * @return Promise<size_t> 写入的字节数，出错时可能小于 len。
   */
  Promise<size_t> write(const char* buf, size_t len) override;

  void close() override { socket_.close(); }

 private:
  friend class Acceptor;

  boost::asio::ip::tcp::socket socket_;
};

using Conn = std::shared_ptr<Socket>;

}  // namespace tcp
}  // namespace coro

#endif  // CORO_INCLUDE_CORO_TCP_CONN_HPP_
