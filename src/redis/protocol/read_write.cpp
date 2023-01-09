#include "coro/redis/protocol/read_write.hpp"

#include "coro/redis/protocol/error.hpp"
#include "coro/redis/protocol/parse.hpp"

namespace coro {
namespace redis {
namespace protocol {

Promise<std::shared_ptr<BulkStringField>> readBulkString(Stream stream,
                                                         size_t len) {
  Promise<std::shared_ptr<BulkStringField>> promise;
  size_t buf_size = len + 2;
  auto buf = std::shared_ptr<char>(new char[buf_size],
                                   [](char* ptr) { delete[] ptr; });

  stream->readn(buf.get(), buf_size)
      .then([promise, buf, buf_size](size_t n) {
        // 读取的 EOF
        if (n != buf_size) {
          std::error_code error(Errc::kEof, errorCategory());
          promise.reject(std::move(error));
          return;
        }
        auto str = std::string(buf.get(), buf_size - 2);
        promise.resolve(BulkStringField::from(std::move(str)));
      })
      .except([promise](size_t n, std::error_code error) {
        promise.reject(std::move(error));
      });

  return promise;
}

Promise<void> readArray(Stream stream, size_t line_len_limit,
                        size_t field_count, std::shared_ptr<ArrayField> array) {
  Promise<void> promise;
  if (field_count == 0) {
    promise.resolve();
    return promise;
  }

  readField(stream, line_len_limit)
      .then([promise, stream, line_len_limit, array,
             field_count](std::shared_ptr<Field> field) {
        array->mut_fields()[field_count - 1] = field;
        readArray(stream, line_len_limit, field_count - 1, array)
            .then([promise]() { promise.resolve(); })
            .except([promise](std::error_code error) {
              promise.reject(std::move(error));
            });
      })
      .except([promise](std::shared_ptr<Field> field, std::error_code error) {
        promise.reject(std::move(error));
      });

  return promise;
}

Promise<std::shared_ptr<Field>> readField(Stream stream,
                                          size_t line_len_limit) {
  Promise<std::shared_ptr<Field>> promise;
  auto buf = std::shared_ptr<char>(new char[line_len_limit],
                                   [](char* ptr) { delete[] ptr; });

  stream->readline(buf.get(), line_len_limit)
      .then([promise, stream, buf, line_len_limit](size_t n) {
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

        switch (buf.get()[0]) {
          case '+': {
            auto result = parseSimpleString(buf.get(), n);
            if (result) {
              promise.resolve(result);
            } else {
              std::error_code error(Errc::kBadMessage, errorCategory());
              promise.reject(std::move(error));
            }
            return;
          }
          case '-': {
            auto result = parseError(buf.get(), n);
            if (result) {
              promise.resolve(result);
            } else {
              std::error_code error(Errc::kBadMessage, errorCategory());
              promise.reject(std::move(error));
            }
            return;
          }
          case ':': {
            auto result = parseInteger(buf.get(), n);
            if (result) {
              promise.resolve(result);
            } else {
              std::error_code error(Errc::kBadMessage, errorCategory());
              promise.reject(std::move(error));
            }
            return;
          }
          case '$': {
            auto len = parseBulkStringLength(buf.get(), n);
            if (len == -2) {
              std::error_code error(Errc::kBadMessage, errorCategory());
              promise.reject(std::move(error));
              return;
            }
            if (len == -1) {
              promise.resolve(BulkStringField::null());
              return;
            }
            readBulkString(stream, len)
                .then([promise](std::shared_ptr<BulkStringField> result) {
                  promise.resolve(result);
                })
                .except([promise](std::shared_ptr<BulkStringField> result,
                                  std::error_code error) {
                  promise.reject(std::move(error));
                });
            return;
          }
          case '*': {
            auto len = parseArrayLength(buf.get(), n);
            if (len == -2) {
              std::error_code error(Errc::kBadMessage, errorCategory());
              promise.reject(std::move(error));
              return;
            }
            if (len == -1) {
              promise.resolve(ArrayField::null());
              return;
            }
            auto array = ArrayField::null();
            array->mut_fields().resize(len);
            readArray(stream, line_len_limit, len, array)
                .then([promise, array]() { promise.resolve(array); })
                .except([promise](std::error_code error) {
                  promise.reject(std::move(error));
                });
            return;
          }
          default:
            std::error_code error(Errc::kBadMessage, errorCategory());
            promise.reject(std::move(error));
        }
      })
      .except([promise](size_t n, std::error_code error) {
        promise.reject(std::move(error));
      });

  return promise;
}

Promise<void> writeField(Stream stream, std::shared_ptr<Field> field) {
  Promise<void> promise;
  auto bytes = std::make_shared<std::string>();
  bytes->reserve(field->bytes());
  field->append(*bytes);
  stream->write(bytes->data(), bytes->length())
      .then([promise](size_t n) { promise.resolve(); })
      .except([promise](size_t n, std::error_code error) {
        promise.reject(std::move(error));
      })
      .finally([bytes]() {});
  return promise;
}

}  // namespace protocol
}  // namespace redis
}  // namespace coro
