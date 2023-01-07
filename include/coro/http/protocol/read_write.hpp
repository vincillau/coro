#ifndef CORO_INCLUDE_CORO_HTTP_PROTOCOL_READ_WRITE_HPP_
#define CORO_INCLUDE_CORO_HTTP_PROTOCOL_READ_WRITE_HPP_

#include "coro/promise.hpp"
#include "coro/stream.hpp"
#include "request.hpp"
#include "response.hpp"

namespace coro {
namespace http {
namespace protocol {

Promise<Request> readReq(Stream stream, size_t line_len_limit = 4096);
Promise<void> writeReq(Stream stream, const Request& req);

Promise<Response> readResp(Stream stream, size_t line_len_limit = 4096);
Promise<void> writeResp(Stream stream, const Response& resp);

}  // namespace protocol
}  // namespace http
}  // namespace coro

#endif  // CORO_INCLUDE_CORO_HTTP_PROTOCOL_READ_WRITE_HPP_
