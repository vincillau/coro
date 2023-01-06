/**
 * @file listener.hpp
 * @brief 表示 TCP 连接监听器的类。
 */

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

  /**
   * @brief 接受一个 TCP 连接。
   * @return Promise<Conn> 接受到的连接。
   */
  Promise<Conn> accept();

 private:
  boost::asio::ip::tcp::acceptor acceptor_;
};

using Listener = std::shared_ptr<Acceptor>;

/**
 * @brief 创建一个监听指定地址和端口的 Listener。
 * @param host 监听的地址。
 * @param port 监听的端口。
 * @param error 错误码，为 nullptr 表示忽略错误。
 * @return Listener 若成功返回 Listener 对象，否则返回 nullptr。
 */
Listener listen(const std::string& host, uint16_t port, std::error_code* error);

/**
 * @brief 创建一个监听指定地址和端口的 Listener。出错时抛出 coro::Exception
 * 异常。
 * @param host 监听的地址。
 * @param port 监听的端口。
 * @return Listener 若成功返回 Listener 对象，否则返回 nullptr。
 */
Listener listen(const std::string& host, uint16_t port);

/**
 * @brief 创建一个监听 127.0.0.1 地址上指定端口的 Listener。
 * @param port 监听的端口。
 * @param error 错误码，为 nullptr 表示忽略错误。
 * @return Listener 若成功返回 Listener 对象，否则返回 nullptr。
 */
inline static Listener listen(uint16_t port, std::error_code* error) {
  return listen("127.0.0.1", port, error);
}

/**
 * @brief 创建一个监听 127.0.0.1 地址上指定端口的 Listener。出错时抛出
 * coro::Exception 异常。
 * @param port 监听的端口。
 * @return Listener 若成功返回 Listener 对象，否则返回 nullptr。
 */
inline static Listener listen(uint16_t port) {
  return listen("127.0.0.1", port);
}

}  // namespace tcp
}  // namespace coro

#endif  // CORO_INCLUDE_CORO_TCP_LISTENER_HPP_
