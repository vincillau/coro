#ifndef CORO_INCLUDE_CORO_SCHED_PROMISE_HPP_
#define CORO_INCLUDE_CORO_SCHED_PROMISE_HPP_

#include <cassert>
#include <functional>
#include <system_error>
#include <vector>

#include "sched.hpp"

namespace coro {
namespace sched {

template <typename T>
class Promise {
 public:
  using Callback = std::function<void()>;

  void resolve(T value);
  void reject(std::error_code error);

  T await(std::error_code* error);

  bool settled() const { return settled_; }
  const std::error_code& error() const { return error_; }

  void pushCallback(Callback callback) {
    callbacks_.emplace_back(std::move(callback));
  }

 private:
  T value_;
  std::error_code error_;
  bool settled_ = false;
  std::vector<Callback> callbacks_;
};

template <typename T>
inline void Promise<T>::resolve(T value) {
  assert(!settled_);
  settled_ = true;
  value_ = std::move(value);
  for (const Callback& cb : callbacks_) {
    cb();
  }
}

template <typename T>
inline void Promise<T>::reject(std::error_code error) {
  assert(!settled_);
  settled_ = true;
  error_ = std::move(error);
  for (const Callback& cb : callbacks_) {
    cb();
  }
}

template <typename T>
inline T Promise<T>::await(std::error_code* error) {
  if (settled_) {
    if (error) {
      *error = std::move(error_);
    }
    return std::move(value_);
  }
  auto coro = Sched::current();
  pushCallback([coro]() { Sched::add(coro); });
  Sched::block();
  if (error) {
    *error = std::move(error_);
  }
  return std::move(value_);
}

template <>
class Promise<void> {
 public:
  using Callback = std::function<void()>;

  void resolve();
  void reject(std::error_code error);

  void await(std::error_code* error);

  bool settled() const { return settled_; }
  const std::error_code& error() const { return error_; }

  void pushCallback(Callback callback) {
    callbacks_.emplace_back(std::move(callback));
  }

 private:
  std::error_code error_;
  bool settled_ = false;
  std::vector<Callback> callbacks_;
};

inline void Promise<void>::resolve() {
  assert(!settled_);
  settled_ = true;
  for (const Callback& cb : callbacks_) {
    cb();
  }
}

inline void Promise<void>::reject(std::error_code error) {
  assert(!settled_);
  settled_ = true;
  error_ = std::move(error);
  for (const Callback& cb : callbacks_) {
    cb();
  }
}

inline void Promise<void>::await(std::error_code* error) {
  if (settled_) {
    if (error) {
      *error = std::move(error_);
    }
    return;
  }
  auto coro = Sched::current();
  pushCallback([coro]() { Sched::add(coro); });
  Sched::block();
  if (error) {
    *error = std::move(error_);
  }
}

}  // namespace sched
}  // namespace coro

#endif  // CORO_INCLUDE_CORO_SCHED_PROMISE_HPP_
