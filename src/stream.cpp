#include "coro/stream.hpp"

#include <cstring>

namespace coro {
namespace impl {

Promise<size_t> Stream::readn(char* buf, size_t len) {
  Promise<size_t> promise;
  if (len == 0) {
    promise.resolve(0);
    return promise;
  }

  read(buf, len)
      .then([this, buf, promise, len](size_t n) {
        // 读到 n 个字节或者读到 EOF 时返回。
        if (n == len || n == 0) {
          promise.resolve(n);
          return;
        }

        // 直到读到 n 个字节。
        size_t nread = n;
        readn(buf + n, len - n)
            .then(
                [promise, len, nread](size_t n) { promise.resolve(nread + n); })
            .except([promise, nread](size_t n, std::error_code error) {
              promise.reject(nread + n, std::move(error));
            });
      })
      .except([promise](size_t n, std::error_code error) {
        promise.reject(n, std::move(error));
      });

  return promise;
}

int64_t findChar(const char* buf, size_t len, char chr) {
  for (int64_t i = 0; i < len; i++) {
    if (buf[i] == chr) {
      return i;
    }
  }
  return -1;
}

Promise<size_t> Stream::readline(char* buf, size_t len) {
  Promise<size_t> promise;
  if (len == 0) {
    promise.resolve(0);
    return promise;
  }

  read(buf, len)
      .then([this, buf, promise, len](size_t n) {
        // 读到 EOF 时返回。
        if (n == 0) {
          promise.resolve(0);
          return;
        }

        int64_t newline_char_index = findChar(buf, n, '\n');
        if (newline_char_index == -1) {
          size_t nread = n;
          readline(buf, len)
              .then([promise, nread](size_t n) { promise.resolve(nread + n); })
              .except([promise, nread](size_t n, std::error_code error) {
                promise.reject(nread + n, std::move(error));
              });
        }

        // 多余的字节追加到 read_buf_。
        size_t line_len = newline_char_index + 1;
        read_buf_.append(buf + line_len, n - line_len);
        promise.resolve(line_len);
      })
      .except([promise](size_t n, std::error_code error) {
        promise.reject(n, std::move(error));
      });

  return promise;
}

size_t Stream::readFromBuf(char* buf, size_t len) {
  if (read_buf_offset_ == read_buf_.size()) {
    return 0;
  }
  size_t available = read_buf_.size() - read_buf_offset_;
  if (available <= len) {
    memcpy(buf, read_buf_.c_str() + read_buf_offset_, available);
    read_buf_.resize(0);
    read_buf_offset_ = 0;
    return available;
  }
  memcpy(buf, read_buf_.c_str() + read_buf_offset_, len);
  read_buf_offset_ += len;
  return len;
}

}  // namespace impl
}  // namespace coro
