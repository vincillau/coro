#ifndef CORO_INCLUDE_CORO_HTTP_PROTOCOL_ERROR_HPP_
#define CORO_INCLUDE_CORO_HTTP_PROTOCOL_ERROR_HPP_

#include <string>
#include <system_error>

namespace coro {
namespace http {
namespace protocol {

enum Errc {
  kEof,
  kLineTooLong,
  kBadStartLine,
  kBadHeader,
};

class ErrorCategory : public std::error_category {
 public:
  ErrorCategory() = default;
  ~ErrorCategory() override = default;

  const char* name() const noexcept override { return "coro::http"; };
  std::string message(int code) const override;
};

const std::error_category& errorCategory();

}  // namespace protocol
}  // namespace http
}  // namespace coro

#endif  // CORO_INCLUDE_CORO_HTTP_PROTOCOL_ERROR_HPP_
