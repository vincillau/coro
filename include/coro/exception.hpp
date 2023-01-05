/**
 * @file exception.hpp
 * @brief 表示各种错误的异常类。
 */

#ifndef CORO_INCLUDE_CORO_EXCEPTION_HPP_
#define CORO_INCLUDE_CORO_EXCEPTION_HPP_

#include <stdexcept>
#include <system_error>

namespace coro {

/**
 * @brief 包装 std::error_code 的异常类。如果调用无参的 await 函数，
 * 在出错时将抛出包装 std::error_code 的 Exception 对象。
 * 协程函数也可以抛出该异常，这将导致协程函数返回的 Promise 被拒绝。
 */
class Exception final {
 public:
  Exception(std::error_code error) : error_(std::move(error)) {}

  const std::error_code& error() const noexcept { return error_; }

 private:
  std::error_code error_;
};

}  // namespace coro

#endif  // CORO_INCLUDE_CORO_EXCEPTION_HPP_
