#ifndef CORO_INCLUDE_CORO_CORO_HPP_
#define CORO_INCLUDE_CORO_CORO_HPP_

#include <type_traits>

#include "exception.hpp"
#include "promise.hpp"
#include "sched/sched.hpp"

namespace coro {

template <typename T, typename Func>
void runFunc(const Func& func, const Promise<T>& promise) {
  promise.resolve(func());
}

template <typename Func>
void runFunc(const Func& func, const Promise<void>& promise) {
  func();
  promise.resolve();
}

template <typename Func>
Promise<typename std::result_of<Func()>::type> spawn(Func func) {
  Promise<typename std::result_of<Func()>::type> promise;
  auto coro_func = [func, promise]() {
    try {
      runFunc(func, promise);
    } catch (const Exception& e) {
      promise.reject(e.error());
    }
  };
  auto coro = std::make_shared<sched::Coro>(std::move(coro_func), 1024 * 64);
  sched::Sched::add(coro);
  return promise;
}

inline static void yield() { sched::Sched::yield(); }

}  // namespace coro

#endif  // CORO_INCLUDE_CORO_CORO_HPP_
