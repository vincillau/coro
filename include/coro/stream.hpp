#ifndef CORO_INCLUDE_CORO_STREAM_HPP_
#define CORO_INCLUDE_CORO_STREAM_HPP_

#include <cstddef>
#include <memory>
#include <string>

#include "promise.hpp"

namespace coro {

class Stream {
 public:
  Stream() = default;
  virtual ~Stream() = default;

  /**
   * @brief 读取最多 len 个字节。
   * @param buf 缓冲区。
   * @param len 缓冲区大小。
   * @return Promise<size_t> 读取到的字节数，可能小于 len。
   */
  virtual Promise<size_t> read(char* buf, size_t len) = 0;

  /**
   * @brief 尽可能读取 len 个字节，除非读取到 EOF 或读取出错。
   * @param buf 缓冲区。
   * @param len 缓冲区大小。
   * @return Promise<size_t> 读取到的字节数，可能小于 len。
   */
  Promise<size_t> readn(char* buf, size_t len);

  /**
   * @brief 尽可能读取一行，以 \n 结尾，除非读取到 EOF 或读取出错。
   * @param buf 缓冲区。
   * @param len 缓冲区大小。
   * @return Promise<size_t> 读取到的字节数，可能小于 len。
   */
  Promise<size_t> readline(char* buf, size_t len);

  /**
   * @brief 向流中写入 len 个字节。
   * @param buf 缓冲区。
   * @param len 缓冲区大小。
   * @return Promise<size_t> 写入的字节数，出错时可能小于 len。
   */
  virtual Promise<size_t> write(const char* buf, size_t len) = 0;

  /**
   * @brief 关闭流。
   */
  virtual void close() = 0;

 protected:
  /**
   * @brief 从流缓冲区中读取至多 len 的字节。
   * @param buf 缓冲区。
   * @param len 缓冲区大小。
   * @return size_t 读取到的字节数，可能小于 len。
   */
  size_t readFromBuf(char* buf, size_t len);

 private:
  // readline 读取到的多于的字节存储在 read_buf_ 中，
  // 子类在读取时应该先调用 readFromBuf 读取上次多读到的数据。
  std::string read_buf_;
  // 尚未被 readFromBuf 读取的字节的偏移量。
  size_t read_buf_offset_ = 0;
};

}  // namespace coro

#endif  // CORO_INCLUDE_CORO_STREAM_HPP_
