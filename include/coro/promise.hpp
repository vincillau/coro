#ifndef CORO_INCLUDE_CORO_PROMISE_HPP_
#define CORO_INCLUDE_CORO_PROMISE_HPP_

#include <memory>

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

  std::shared_ptr<sched::Promise<T>> shared() const { return promise_; }
  std::weak_ptr<sched::Promise<T>> weak() const {
    return std::weak_ptr<sched::Promise<T>>(promise_);
  }

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

  std::shared_ptr<sched::Promise<void>> shared() const { return promise_; }
  std::weak_ptr<sched::Promise<void>> weak() const {
    return std::weak_ptr<sched::Promise<void>>(promise_);
  }

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

template <typename... Promises>
Promise<size_t> all(Promises... promises);

template <typename... Promises>
Promise<void> allSettled(Promises... promises);

inline static void anyRecursive(Promise<int64_t> result,
                                std::shared_ptr<size_t> pending_count,
                                size_t index) {}

template <typename P, typename... Promises>
void anyRecursive(Promise<int64_t> result,
                  std::shared_ptr<size_t> pending_count, size_t index, P first,
                  Promises... promises) {
  if (first.shared()->settled()) {
    if (!first.shared()->error()) {
      result.resolve(index);
      return;
    }
    (*pending_count)--;
    if (*pending_count == 0) {
      result.resolve(-1);
      return;
    }
  }

  auto weak_result = result.weak();
  auto weak_first = first.weak();
  auto onSettle = [weak_result, pending_count, index, weak_first]() {
    auto shared_result = weak_result.lock();
    auto shared_first = weak_first.lock();

    if (!shared_result || shared_result->settled()) {
      return;
    }
    if (!shared_first->error()) {
      shared_result->resolve(index);
      return;
    }
    (*pending_count)--;
    if (*pending_count == 0) {
      shared_result->resolve(-1);
      return;
    }
  };
  first.shared()->pushOnSettle(std::move(onSettle));

  anyRecursive(result, pending_count, index + 1, promises...);
}

template <typename... Promises>
Promise<int64_t> any(Promises... promises) {
  Promise<int64_t> result;
  auto pending_count = std::make_shared<size_t>(sizeof...(promises));
  anyRecursive(result, pending_count, 0, promises...);
  return result;
}

template <typename... Promises>
Promise<size_t> race(Promises... promises);

#define ANY(...) switch (coro::any(__VA_ARGS__).await())

}  // namespace coro

#endif  // CORO_INCLUDE_CORO_PROMISE_HPP_
