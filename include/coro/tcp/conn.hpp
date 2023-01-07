#ifndef CORO_INCLUDE_CORO_TCP_CONN_HPP_
#define CORO_INCLUDE_CORO_TCP_CONN_HPP_

#include <boost/asio.hpp>

#include "coro/promise.hpp"
#include "coro/stream.hpp"

namespace coro {
namespace tcp {

namespace impl {

/**
 * @brief 表示 TCP 连接的类。
 */
class Conn : public coro::impl::Stream {
 public:
  explicit Conn(boost::asio::io_context& io_context) : socket_(io_context) {}

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

  /**
   * @brief 关闭连接。
   */
  void close() override { socket_.close(); }

 private:
  friend class Listener;
  friend Promise<std::shared_ptr<Conn>> connect(const std::string& host,
                                                uint16_t port);

  boost::asio::ip::tcp::socket socket_;
};

Promise<std::shared_ptr<Conn>> connect(const std::string& host, uint16_t port);

}  // namespace impl

using Conn = std::shared_ptr<impl::Conn>;
using impl::connect;  // NOLINT

}  // namespace tcp
}  // namespace coro

#endif  // CORO_INCLUDE_CORO_TCP_CONN_HPP_
