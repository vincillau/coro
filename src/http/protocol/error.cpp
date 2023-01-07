#include "coro/http/protocol/error.hpp"

namespace coro {
namespace http {
namespace protocol {

std::string ErrorCategory::message(int code) const {
  switch (code) {
    case Errc::kEof:
      return "End of file";
    case Errc::kLineTooLong:
      return "Line is too long";
    case Errc::kBadStartLine:
      return "Bad start line";
    case Errc::kBadHeader:
      return "Bad header";
    default:
      return "Unknown error";
  }
}

const std::error_category& errorCategory() {
  static ErrorCategory instance;
  return instance;
}

}  // namespace protocol
}  // namespace http
}  // namespace coro
