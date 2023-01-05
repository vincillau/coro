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

  Promise() = default;
  Promise(const Promise&) = delete;
  bool operator=(const Promise&) = delete;
  Promise(Promise&&) = delete;
  bool operator=(Promise&&) = delete;
  ~Promise();

  void resolve(T value);
  void reject(std::error_code error);

  T await(std::error_code* error);

  bool settled() const { return settled_; }
  const std::error_code& error() const { return error_; }

  void pushOnSettle(Callback callback) {
    onSettle_.emplace_back(std::move(callback));
  }

  void pushOnDestory(Callback callback) {
    onDestory_.emplace_back(std::move(callback));
  }

 private:
  T value_;
  std::error_code error_;
  bool settled_ = false;
  std::vector<Callback> onSettle_;
  std::vector<Callback> onDestory_;
};

template <typename T>
inline Promise<T>::~Promise() {
  if (!settled()) {
    std::error_code error(static_cast<int>(std::errc::operation_canceled),
                          std::system_category());
    reject(std::move(error));
  }
  for (const Callback& cb : onDestory_) {
    cb();
  }
}

template <typename T>
inline void Promise<T>::resolve(T value) {
  assert(!settled());
  settled_ = true;
  value_ = std::move(value);
  for (const Callback& cb : onSettle_) {
    cb();
  }
}

template <typename T>
inline void Promise<T>::reject(std::error_code error) {
  assert(!settled());
  settled_ = true;
  error_ = std::move(error);
  for (const Callback& cb : onSettle_) {
    cb();
  }
}

template <typename T>
inline T Promise<T>::await(std::error_code* error) {
  if (settled()) {
    if (error) {
      *error = std::move(error_);
    }
    return std::move(value_);
  }
  auto coro = Sched::current();
  pushOnSettle([coro]() { Sched::add(coro); });
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

  Promise() = default;
  Promise(const Promise&) = delete;
  bool operator=(const Promise&) = delete;
  Promise(Promise&&) = delete;
  bool operator=(Promise&&) = delete;
  ~Promise();

  void resolve();
  void reject(std::error_code error);

  void await(std::error_code* error);

  bool settled() const { return settled_; }
  const std::error_code& error() const { return error_; }

  void pushOnSettle(Callback callback) {
    onSettle_.emplace_back(std::move(callback));
  }

  void pushOnDestory(Callback callback) {
    onDestory_.emplace_back(std::move(callback));
  }

 private:
  std::error_code error_;
  bool settled_ = false;
  std::vector<Callback> onSettle_;
  std::vector<Callback> onDestory_;
};

inline Promise<void>::~Promise() {
  if (!settled()) {
    std::error_code error(static_cast<int>(std::errc::operation_canceled),
                          std::system_category());
    reject(std::move(error));
  }
  for (const Callback& cb : onDestory_) {
    cb();
  }
}

inline void Promise<void>::resolve() {
  assert(!settled());
  settled_ = true;
  for (const Callback& cb : onSettle_) {
    cb();
  }
}

inline void Promise<void>::reject(std::error_code error) {
  assert(!settled());
  settled_ = true;
  error_ = std::move(error);
  for (const Callback& cb : onSettle_) {
    cb();
  }
}

inline void Promise<void>::await(std::error_code* error) {
  if (settled()) {
    if (error) {
      *error = std::move(error_);
    }
    return;
  }
  auto coro = Sched::current();
  pushOnSettle([coro]() { Sched::add(coro); });
  Sched::block();
  if (error) {
    *error = std::move(error_);
  }
}

}  // namespace sched
}  // namespace coro

#endif  // CORO_INCLUDE_CORO_SCHED_PROMISE_HPP_
