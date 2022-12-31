#ifndef CORO_INCLUDE_CORO_PROMISE_HPP_
#define CORO_INCLUDE_CORO_PROMISE_HPP_

#include <memory>
#include <type_traits>

#include "exception.hpp"
#include "sched/promise.hpp"

namespace coro {

template <typename T>
class Promise {
 public:
  Promise() : promise_(std::make_shared<sched::Promise<T>>()) {}

  void resolve(T value) const { promise_->resolve(std::move(value)); }
  void reject(std::error_code error) const {
    promise_->reject(std::move(error));
  }

  T await() const;
  T await(std::error_code* error) const { return promise_->await(error); }

 private:
  std::shared_ptr<sched::Promise<T>> promise_;
};

template <typename T>
inline T Promise<T>::await() const {
  std::error_code error;
  auto result = await(&error);
  if (error) {
    throw Exception(std::move(error));
  }
  return std::move(result);
}

template <>
class Promise<void> {
 public:
  Promise() : promise_(std::make_shared<sched::Promise<void>>()) {}

  void resolve() const { promise_->resolve(); }
  void reject(std::error_code error) const {
    promise_->reject(std::move(error));
  }

  void await() const;
  void await(std::error_code* error) const { promise_->await(error); }

 private:
  std::shared_ptr<sched::Promise<void>> promise_;
};

inline void Promise<void>::await() const {
  std::error_code error;
  await(&error);
  if (error) {
    throw Exception(std::move(error));
  }
}

}  // namespace coro

#endif  // CORO_INCLUDE_CORO_PROMISE_HPP_
