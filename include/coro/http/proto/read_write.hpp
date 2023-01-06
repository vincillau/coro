#ifndef CORO_INCLUDE_CORO_HTTP_PROTO_READ_WRITE_HPP_
#define CORO_INCLUDE_CORO_HTTP_PROTO_READ_WRITE_HPP_

#include "coro/promise.hpp"
#include "coro/stream.hpp"
#include "request.hpp"
#include "response.hpp"

namespace coro {
namespace http {
namespace proto {

Promise<Request> readReq(std::shared_ptr<Stream> stream,
                         size_t line_len_limit = 4096);
Promise<void> writeReq(std::shared_ptr<Stream> stream, const Request& req);

Promise<Response> readResp(std::shared_ptr<Stream> stream,
                           size_t line_len_limit = 4096);
Promise<void> writeResp(std::shared_ptr<Stream> stream, const Response& resp);

}  // namespace proto
}  // namespace http
}  // namespace coro

#endif  // CORO_INCLUDE_CORO_HTTP_PROTO_READ_WRITE_HPP_
