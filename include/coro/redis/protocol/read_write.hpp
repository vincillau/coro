#ifndef CORO_INCLUDE_CORO_REDIS_PROTOCOL_READ_WRITE_HPP_
#define CORO_INCLUDE_CORO_REDIS_PROTOCOL_READ_WRITE_HPP_

#include <memory>

#include "coro/promise.hpp"
#include "coro/stream.hpp"
#include "field.hpp"

namespace coro {
namespace redis {
namespace protocol {

Promise<std::shared_ptr<Field>> readField(Stream stream,
                                          size_t line_len_limit = 4096);
Promise<void> writeField(Stream stream, std::shared_ptr<Field> field);

}  // namespace protocol
}  // namespace redis
}  // namespace coro

#endif  // CORO_INCLUDE_CORO_REDIS_PROTO_COLREAD_WRITE_HPP_
