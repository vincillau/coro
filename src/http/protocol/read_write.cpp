#include "coro/http/protocol/read_write.hpp"

#include "coro/http/protocol/error.hpp"
#include "coro/http/protocol/parse.hpp"

namespace coro {
namespace http {
namespace protocol {

using Headers = std::vector<std::pair<std::string, std::string>>;

static Promise<void> readHeaders(std::shared_ptr<Stream> stream,
                                 size_t line_len_limit, Headers* headers,
                                 std::shared_ptr<char> buf) {
  Promise<void> promise;

  stream->readline(buf.get(), line_len_limit)
      .then([promise, stream, line_len_limit, headers, buf](size_t n) {
        // 超出行长限制。
        if (n == line_len_limit && buf.get()[n - 1] != '\n') {
          std::error_code error(Errc::kLineTooLong, errorCategory());
          promise.reject(std::move(error));
          return;
        }
        // 读取的 EOF
        if (n == 0 || buf.get()[n - 1] != '\n') {
          std::error_code error(Errc::kEof, errorCategory());
          promise.reject(std::move(error));
          return;
        }

        // 读取到空行则结束。
        if (n == 2 && buf.get()[0] == '\r' && buf.get()[1] == '\n') {
          promise.resolve();
          return;
        }

        std::string name, value;
        bool ok = parseHeader(buf.get(), name, value);
        // 标头行格式错误。
        if (!ok) {
          std::error_code error(Errc::kBadHeader, errorCategory());
          promise.reject(std::move(error));
          return;
        }
        headers->push_back({std::move(name), std::move(value)});

        readHeaders(stream, line_len_limit, headers, buf)
            .then([promise]() { promise.resolve(); })
            .except([promise](std::error_code error) {
              promise.reject(std::move(error));
            });
      })
      .except([promise](size_t n, std::error_code error) {
        promise.reject(std::move(error));
      });

  return promise;
}

Promise<Request> readReq(std::shared_ptr<Stream> stream,
                         size_t line_len_limit) {
  Promise<Request> promise;
  auto buf = std::shared_ptr<char>(new char[line_len_limit],
                                   [](char* ptr) { delete[] ptr; });
  auto req = std::make_shared<Request>();

  stream->readline(buf.get(), line_len_limit)
      .then([promise, buf, stream, line_len_limit, req](size_t n) {
        // 超出行长限制。
        if (n == line_len_limit && buf.get()[n - 1] != '\n') {
          std::error_code error(Errc::kLineTooLong, errorCategory());
          promise.reject(std::move(error));
          return;
        }
        // 读取的 EOF
        if (n == 0 || buf.get()[n - 1] != '\n') {
          std::error_code error(Errc::kEof, errorCategory());
          promise.reject(std::move(error));
          return;
        }

        bool ok =
            parseReqStartLine(buf.get(), req->method, req->url, req->version);
        // 起始行格式错误。
        if (!ok) {
          std::error_code error(Errc::kBadStartLine, errorCategory());
          promise.reject(std::move(error));
          return;
        }

        readHeaders(stream, line_len_limit, &req->headers, buf)
            .then([promise, req]() { promise.resolve(std::move(*req)); })
            .except([promise](std::error_code error) {
              promise.reject(std::move(error));
            });
      })
      .except([promise](size_t n, std::error_code error) {
        promise.reject(std::move(error));
      });
  return promise;
}

Promise<void> writeReq(std::shared_ptr<Stream> stream, const Request& req) {
  Promise<void> promise;

  auto bytes = std::make_shared<std::string>();
  bytes->append(req.method);
  bytes->push_back(' ');
  bytes->append(req.url);
  bytes->push_back(' ');
  bytes->append(req.version);
  bytes->append("\r\n");
  for (auto iter = req.headers.cbegin(); iter != req.headers.cend(); iter++) {
    bytes->append(iter->first);
    bytes->append(": ");
    bytes->append(iter->second);
    bytes->append("\r\n");
  }
  bytes->append("\r\n");

  stream->write(bytes->data(), bytes->length())
      .then([promise](size_t n) { promise.resolve(); })
      .except([promise](size_t n, std::error_code error) {
        promise.reject(std::move(error));
      })
      .finally([bytes]() {});

  return promise;
}

Promise<Response> readResp(std::shared_ptr<Stream> stream,
                           size_t line_len_limit) {
  Promise<Response> promise;
  auto buf = std::shared_ptr<char>(new char[line_len_limit],
                                   [](char* ptr) { delete[] ptr; });
  auto resp = std::make_shared<Response>();

  stream->readline(buf.get(), line_len_limit)
      .then([promise, buf, stream, line_len_limit, resp](size_t n) {
        // 超出行长限制。
        if (n == line_len_limit && buf.get()[n - 1] != '\n') {
          std::error_code error(Errc::kLineTooLong, errorCategory());
          promise.reject(std::move(error));
          return;
        }
        // 读取的 EOF
        if (n == 0 || buf.get()[n - 1] != '\n') {
          std::error_code error(Errc::kEof, errorCategory());
          promise.reject(std::move(error));
          return;
        }

        bool ok = parseRespStartLine(buf.get(), resp->version, resp->code,
                                     resp->reason);
        // 起始行格式错误。
        if (!ok) {
          std::error_code error(Errc::kBadStartLine, errorCategory());
          promise.reject(std::move(error));
          return;
        }

        readHeaders(stream, line_len_limit, &resp->headers, buf)
            .then([promise, resp]() { promise.resolve(std::move(*resp)); })
            .except([promise](std::error_code error) {
              promise.reject(std::move(error));
            });
      })
      .except([promise](size_t n, std::error_code error) {
        promise.reject(std::move(error));
      });
  return promise;
}

Promise<void> writeResp(std::shared_ptr<Stream> stream, const Response& resp) {
  Promise<void> promise;

  auto bytes = std::make_shared<std::string>();
  bytes->append(resp.version);
  bytes->push_back(' ');
  bytes->append(std::to_string(resp.code));
  bytes->push_back(' ');
  bytes->append(resp.reason);
  bytes->append("\r\n");
  for (auto iter = resp.headers.cbegin(); iter != resp.headers.cend(); iter++) {
    bytes->append(iter->first);
    bytes->append(": ");
    bytes->append(iter->second);
    bytes->append("\r\n");
  }
  bytes->append("\r\n");

  stream->write(bytes->data(), bytes->length())
      .then([promise](size_t n) { promise.resolve(); })
      .except([promise](size_t n, std::error_code error) {
        promise.reject(std::move(error));
      })
      .finally([bytes]() {});

  return promise;
}

}  // namespace protocol
}  // namespace http
}  // namespace coro
