/**
 * @file coro.hpp
 * @brief 协程类
 */

#ifndef CORO_INCLUDE_CORO_SCHED_CORO_HPP_
#define CORO_INCLUDE_CORO_SCHED_CORO_HPP_

#include <cstddef>
#include <functional>

#include "fctx.hpp"

namespace coro {
namespace sched {

/**
 * @brief Coro 表示一个协程对象，它在一个由 allocStack 分配的栈空间内运行
 * Coro::Func 类型的函数。
 */
class Coro {
 public:
  /**
   * @brief 协程函数类型。协程函数禁止抛出异常。
   */
  using Func = std::function<void()>;

  /**
   * @brief 构造一个 Coro 对象，但没有与之关联的协程函数，也不为其分配栈。
   * 用于表示线程中的第一个协程。
   */
  Coro() = default;
  /**
   * @brief 构造一个 Coro 对象，指定其运行的函数和栈大小。
   * @param func 协程函数。
   * @param stack_size 栈大小。
   */
  Coro(Func func, size_t stack_size);

  // Coro 对象禁止拷贝和移动，所有的 Coro 对象都由 std::shared_ptr 持有。
  Coro(const Coro&) = delete;
  Coro& operator=(const Coro&) = delete;
  Coro(Coro&&) = delete;
  Coro& operator=(Coro&&) = delete;

  /**
   * @brief 销毁 Coro 对象，释放其栈内存（如果有）。
   * 必须在该协程停止后由另一个协程中调用。
   */
  ~Coro();

  /**
   * @brief 恢复协程，当前的协程将放弃 CPU。
   * @param prev 被恢复的协程的前一个协程，也就是当前正在运行的协程。
   */
  void resume(Coro* prev) const;

 private:
  /**
   * @brief 协程函数的包装器，该函数将调用实际的协程函数。
   * 由于协程函数禁止抛出异常，所以该函数在捕获到异常时数据错误信息并退出程序。
   * @param trans trans.data 是指向上一个协程对象的指针。
   */
  static void funcWrapper(transfer_t trans);

  Func func_;                  // 协程函数。
  fcontext_t fctx_ = nullptr;  // fcontext 上下文。
  // 指向栈底的指针。等于 nullptr 则表示未为该协程分配栈。
  // 只有线程中的第一个协程没有分配栈（由操作系统分配）。
  // 每个线程在启动时被认为是只有一个协程的线程。
  void* stack_ = nullptr;
  size_t stack_size_ = 0;  // 栈大小。
};

}  // namespace sched
}  // namespace coro

#endif  // CORO_INCLUDE_CORO_SCHED_CORO_HPP_
