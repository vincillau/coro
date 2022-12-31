#ifndef CORO_INCLUDE_CORO_SCHED_PROMISE_HPP_
#define CORO_INCLUDE_CORO_SCHED_PROMISE_HPP_

#include <functional>
#include <system_error>

#include "sched.hpp"

namespace coro {
namespace sched {

template <typename T>
class Promise {
 public:
  void resolve(T value);
  void reject(std::error_code error);

  T await(std::error_code* error);

 private:
  T value_;
  std::error_code error_;
  bool settled_ = false;
  std::function<void()> onSettle_;
};

template <typename T>
inline void Promise<T>::resolve(T value) {
  settled_ = true;
  value_ = std::move(value);
  if (onSettle_) {
    onSettle_();
  }
}

template <typename T>
inline void Promise<T>::reject(std::error_code error) {
  settled_ = true;
  error_ = std::move(error);
  if (onSettle_) {
    onSettle_();
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
  onSettle_ = [coro]() { Sched::add(coro); };
  Sched::block();
  if (error) {
    *error = std::move(error_);
  }
  return std::move(value_);
}

template <>
class Promise<void> {
 public:
  void resolve();
  void reject(std::error_code error);

  void await(std::error_code* error);

 private:
  std::error_code error_;
  bool settled_ = false;
  std::function<void()> onSettle_;
};

inline void Promise<void>::resolve() {
  settled_ = true;
  if (onSettle_) {
    onSettle_();
  }
}

inline void Promise<void>::reject(std::error_code error) {
  settled_ = true;
  error_ = std::move(error);
  if (onSettle_) {
    onSettle_();
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
  onSettle_ = [coro, this]() { Sched::add(coro); };
  Sched::block();
  if (error) {
    *error = std::move(error_);
  }
}

}  // namespace sched
}  // namespace coro

#endif  // CORO_INCLUDE_CORO_SCHED_PROMISE_HPP_
