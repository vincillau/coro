/**
 * @file spawn.hpp
 * @brief spawn 函数的实现。
 */

#ifndef CORO_INCLUDE_CORO_SPAWN_HPP_
#define CORO_INCLUDE_CORO_SPAWN_HPP_

#include <type_traits>

#include "exception.hpp"
#include "promise.hpp"
#include "sched/sched.hpp"

namespace coro {

/**
 * @brief 协程函数包装器。
 * 在协程函数返回时将 Promise 标记为已兑现。
 * 在捕获到协程函数抛出的异常时将 Promise 标记为已拒绝。
 * @tparam Func 函数类型。
 * @tparam Ret 函数返回值类型。
 * @param func 协程函数。
 * @param promise 表示协程函数返回值的 Promise。
 */
template <typename Func, typename Ret>
inline void coroFuncWrapper(const Func& func, Promise<Ret> promise) {
  try {
    promise.resolve(func());
  } catch (const Exception& e) {
    promise.reject(e.error());
  }
}

template <typename Func>
inline static void coroFuncWrapper(const Func& func, Promise<void> promise) {
  try {
    func();
    promise.resolve();
  } catch (const Exception& e) {
    promise.reject(e.error());
  }
}

// 协程栈默认大小，单位字节。
static constexpr size_t kDefaultStackSize = 64 * 1024;

/**
 * @brief 创建一个新的协程。
 * @tparam Func 协程函数类型。
 * @param func 新协程执行的函数。
 * @param stack_size 栈大小，单位字节。
 * @return Promise<typename std::result_of<Func()>::type> 协程函数执行结果的
 * Promise，在协程函数返回时敲定。
 */
template <typename Func>
inline Promise<typename std::result_of<Func()>::type> spawn(
    Func func, size_t stack_size = kDefaultStackSize) {
  Promise<typename std::result_of<Func()>::type> promise;
  auto coro = std::make_shared<sched::Coro>(
      [func, promise]() { coroFuncWrapper(func, promise); }, stack_size);
  sched::schedule(coro);
  return promise;
}

}  // namespace coro

#endif  // CORO_INCLUDE_CORO_SPAWN_HPP_
