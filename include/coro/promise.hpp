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
  template <typename P, typename... Promises>
  friend void anyImpl(Promise<int64_t> result,
                      std::shared_ptr<size_t> pending_count, size_t index,
                      P first, Promises... promises);

  std::shared_ptr<sched::Promise<T>> promise() const { return promise_; }

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
  template <typename P, typename... Promises>
  friend void anyImpl(Promise<int64_t> result,
                      std::shared_ptr<size_t> pending_count, size_t index,
                      P first, Promises... promises);

  std::shared_ptr<sched::Promise<void>> promise() const { return promise_; }

  std::shared_ptr<sched::Promise<void>> promise_;
};

inline void Promise<void>::await() const {
  std::error_code error;
  await(&error);
  if (error) {
    throw Exception(std::move(error));
  }
}

template <typename... Promises>
Promise<size_t> all(Promises... promises);

template <typename... Promises>
Promise<void> allSettled(Promises... promises);

inline static void anyImpl(Promise<int64_t> result,
                           std::shared_ptr<size_t> pending_count,
                           size_t index) {}

template <typename P, typename... Promises>
void anyImpl(Promise<int64_t> result, std::shared_ptr<size_t> pending_count,
             size_t index, P first, Promises... promises) {
  if (first.promise()->settled()) {
    if (!first.promise()->error()) {
      result.resolve(index);
      return;
    }
    (*pending_count)--;
    if (*pending_count == 0) {
      result.resolve(-1);
      return;
    }
  }

  auto callback = [result, pending_count, index, first]() {
    if (result.promise()->settled()) {
      return;
    }
    if (!first.promise()->error()) {
      result.resolve(index);
      return;
    }
    (*pending_count)--;
    if (*pending_count == 0) {
      result.resolve(-1);
      return;
    }
  };
  first.promise()->pushCallback(std::move(callback));

  anyImpl(result, pending_count, index + 1, promises...);
}

template <typename... Promises>
Promise<int64_t> any(Promises... promises) {
  Promise<int64_t> result;
  auto pending_count = std::make_shared<size_t>(sizeof...(promises));
  anyImpl(result, pending_count, 0, promises...);
  return result;
}

template <typename... Promises>
Promise<size_t> race(Promises... promises);

}  // namespace coro

#endif  // CORO_INCLUDE_CORO_PROMISE_HPP_
