#ifndef CORO_INCLUDE_CORO_EXCEPTION_HPP_
#define CORO_INCLUDE_CORO_EXCEPTION_HPP_

#include <stdexcept>
#include <system_error>

namespace coro {

class Exception {
 public:
  Exception(std::error_code error) : error_(std::move(error)) {}

  const std::error_code& error() const { return error_; }

 private:
  std::error_code error_;
};

}  // namespace coro

#endif  // CORO_INCLUDE_CORO_EXCEPTION_HPP_
