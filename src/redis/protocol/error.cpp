#include "coro/redis/protocol/error.hpp"

namespace coro {
namespace redis {
namespace protocol {

std::string ErrorCategory::message(int code) const {
  switch (code) {
    case Errc::kEof:
      return "End of file";
    case Errc::kLineTooLong:
      return "Line is too long";
    case Errc::kBadMessage:
      return "Bad message";
    default:
      return "Unknown error";
  }
}

const std::error_category& errorCategory() {
  static ErrorCategory instance;
  return instance;
}

}  // namespace protocol
}  // namespace redis
}  // namespace coro
