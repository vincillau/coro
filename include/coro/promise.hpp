#ifndef CORO_INCLUDE_CORO_PROMISE_HPP_
#define CORO_INCLUDE_CORO_PROMISE_HPP_

#include <cassert>
#include <memory>

#include "exception.hpp"
#include "sched/promise.hpp"

namespace coro {

/**
 * @brief coro::sched::Promise 的包装器。
 *
 * @tparam T 异步函数返回值类型。
 */
template <typename T>
class Promise {
 public:
  /**
   * @brief 构造一个 Promise 对象，该 Promise 对象处于待定状态。
   */
  Promise() : promise_(std::make_shared<sched::Promise<T>>()) {}

  /**
   * @brief 将 Promise 设置为已兑现，并将 value 设置为异步函数的执行结果。
   * 调用此函数前 Promise 必须处于待定状态。
   * @param value 异步函数的执行结果。
   */
  void resolve(T value) const { promise_->resolve(std::move(value)); }

  /**
   * @brief 将 Promise 设置为已拒绝，并指定错误码。
   * 调用此函数前 Promise 必须处于待定状态。
   * @param error 错误码。
   */
  void reject(std::error_code error) const {
    assert(error);
    promise_->reject(std::move(error));
  }

  /**
   * @brief 将 Promise 设置为已拒绝，并指定错误码。
   * 调用此函数前 Promise 必须处于待定状态。
   * @param value 异步函数执行结果。
   * @param error 错误码。
   */
  void reject(T value, std::error_code error) const {
    assert(error);
    promise_->reject(std::move(value), std::move(error));
  }

  /**
   * @brief 如果 Promise 未敲定，则阻塞当前协程直至 Promise
   * 敲定，返回异步函数执行结果。
   * 如果 Promise 被标记为已拒绝，则会抛出 coro::Exception。
   * Promise 只能调用 await 一次。
   * @return T Promise 的执行结果，如果 Promise
   * 被设置为已拒绝，返回值将没有意义。
   */
  T await() const;

  /**
   * @brief 如果 Promise 未敲定，则阻塞当前协程直至 Promise
   * 敲定，返回异步函数执行结果。Promise 只能调用 await 一次。
   * @param error Promise 的错误码，为 nullptr 则忽略错误。
   * @return T Promise 的执行结果。
   */
  T await(std::error_code* error) const;

  /**
   * @brief Promise 被兑现时调用指定回调。
   * @param callback 回调函数，value 为异步函数执行结果。
   */
  Promise<T>& then(std::function<void(T value)> callback);

  /**
   * @brief Promise 被拒绝时调用指定回调。
   * @param callback 回调函数，value 为异步函数执行结果，error 为错误码。
   */
  Promise<T>& except(
      std::function<void(T value, std::error_code error)> callback);

  /**
   * @brief Promise 被敲定时调用指定回调。
   * @param callback 回调函数。
   */
  Promise<T>& finally(std::function<void()> callback);

  template <typename P, typename... Promises>
  friend void allUnpack(Promise<int> result,
                        std::shared_ptr<size_t> pending_count, size_t index,
                        P first, Promises... promises);

  template <typename P, typename... Promises>
  friend void allSettledUnpack(Promise<void> result,
                               std::shared_ptr<size_t> pending_count,
                               size_t index, P first, Promises... promises);

  template <typename P, typename... Promises>
  friend void anyUnpack(Promise<int> result,
                        std::shared_ptr<size_t> pending_count, size_t index,
                        P first, Promises... promises);

  template <typename P, typename... Promises>
  friend void raceUnpack(Promise<size_t> result, size_t index, P first,
                         Promises... promises);

 private:
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

template <typename T>
inline T Promise<T>::await(std::error_code* error) const {
  std::error_code err;
  T value = promise_->await(err);
  if (error) {
    *error = std::move(err);
  }
  return std::move(value);
}

template <typename T>
inline Promise<T>& Promise<T>::then(std::function<void(T)> callback) {
  promise_->then(std::move(callback));
  return *this;
}

template <typename T>
inline Promise<T>& Promise<T>::except(
    std::function<void(T value, std::error_code)> callback) {
  promise_->except(std::move(callback));
  return *this;
}

template <typename T>
inline Promise<T>& Promise<T>::finally(std::function<void()> callback) {
  promise_->finally(std::move(callback));
  return *this;
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
  void await(std::error_code* error) const;

  Promise<void>& then(std::function<void()> callback);
  Promise<void>& except(std::function<void(std::error_code error)> callback);
  Promise<void>& finally(std::function<void()> callback);

  template <typename P, typename... Promises>
  friend void allUnpack(Promise<int> result,
                        std::shared_ptr<size_t> pending_count, size_t index,
                        P first, Promises... promises);

  template <typename P, typename... Promises>
  friend void allSettledUnpack(Promise<void> result,
                               std::shared_ptr<size_t> pending_count,
                               size_t index, P first, Promises... promises);

  template <typename P, typename... Promises>
  friend void anyUnpack(Promise<int> result,
                        std::shared_ptr<size_t> pending_count, size_t index,
                        P first, Promises... promises);

  template <typename P, typename... Promises>
  friend void raceUnpack(Promise<size_t> result, size_t index, P first,
                         Promises... promises);

 private:
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

inline void Promise<void>::await(std::error_code* error) const {
  std::error_code err;
  promise_->await(err);
  if (error) {
    *error = std::move(err);
  }
}

inline Promise<void>& Promise<void>::then(std::function<void()> callback) {
  promise_->then(std::move(callback));
  return *this;
}

inline Promise<void>& Promise<void>::except(
    std::function<void(std::error_code error)> callback) {
  promise_->except(std::move(callback));
  return *this;
}

inline Promise<void>& Promise<void>::finally(std::function<void()> callback) {
  promise_->finally(std::move(callback));
  return *this;
}

inline static void allUnpack(Promise<int> result,
                             std::shared_ptr<size_t> pending_count,
                             size_t index) {}

template <typename P, typename... Promises>
inline void allUnpack(Promise<int> result,
                      std::shared_ptr<size_t> pending_count, size_t index,
                      P first, Promises... promises) {
  auto p = first.promise();
  if (p->settled()) {
    if (p->err()) {
      result.resolve(index);
      return;
    }
    (*pending_count)--;
    if (*pending_count == 0) {
      result.resolve(-1);
      return;
    }
  }

  auto on_settle = [result, pending_count, index, first]() {
    auto p = first.promise();
    if (p->settled()) {
      return;
    }
    if (p->err()) {
      result.resolve(index);
    }
    (*pending_count)--;
    if (*pending_count == 0) {
      result.resolve(-1);
    }
  };
  p->appendOnSettle(std::move(on_settle));

  allUnpack(result, pending_count, index + 1, promises...);
}

/**
 * @brief 返回一个新的 promise 对象，等到所有的 promise 对象都成功或有任意一个
 * promise 失败。
 * @tparam Promises Promise 类型。
 * @param promises Promise 集合。
 * @return Promise<int> 返回失败的 promise 的下标，如果都成功则返回 -1。
 */
template <typename... Promises>
inline Promise<int> all(Promises... promises) {
  Promise<int> result;
  auto pending_count = std::make_shared<size_t>(sizeof...(promises));
  allUnpack(result, pending_count, 0, promises...);
  return result;
}

inline static void allSettledUnpack(Promise<void> result,
                                    std::shared_ptr<size_t> pending_count,
                                    size_t index) {}

template <typename P, typename... Promises>
inline void allSettledUnpack(Promise<void> result,
                             std::shared_ptr<size_t> pending_count,
                             size_t index, P first, Promises... promises) {
  auto p = first.promise();
  if (p->settled()) {
    (*pending_count)--;
    if (*pending_count == 0) {
      result.resolve();
      return;
    }
  }

  auto on_settle = [result, pending_count, index, first]() {
    auto p = first.promise();
    if (p->settled()) {
      return;
    }
    (*pending_count)--;
    if (*pending_count == 0) {
      result.resolve();
    }
  };
  p->appendOnSettle(std::move(on_settle));

  allSettledUnpack(result, pending_count, index + 1, promises...);
}

/**
 * @brief 等到所有 promise 都已敲定（每个 promise 都已兑现或已拒绝）。
 * @tparam Promises Promise 类型。
 * @param promises Promise 集合。
 * @return Promise<void> 表示所有 promise 都敲定的 promise。
 */
template <typename... Promises>
inline Promise<void> allSettled(Promises... promises) {
  Promise<void> result;
  auto pending_count = std::make_shared<size_t>(sizeof...(promises));
  allSettledUnpack(result, pending_count, 0, promises...);
  return result;
}

inline static void anyUnpack(Promise<int> result,
                             std::shared_ptr<size_t> pending_count,
                             size_t index) {}

template <typename P, typename... Promises>
inline void anyUnpack(Promise<int> result,
                      std::shared_ptr<size_t> pending_count, size_t index,
                      P first, Promises... promises) {
  auto p = first.promise();
  if (p->settled()) {
    if (!p->err()) {
      result.resolve(index);
    }
    (*pending_count)--;
    if (*pending_count == 0) {
      result.resolve(-1);
      return;
    }
  }

  auto on_settle = [result, pending_count, index, first]() {
    auto p = first.promise();
    if (p->settled()) {
      return;
    }
    if (!p->err()) {
      result.resolve(index);
    }
    (*pending_count)--;
    if (*pending_count == 0) {
      result.resolve(-1);
    }
  };
  p->appendOnSettle(std::move(on_settle));

  anyUnpack(result, pending_count, index + 1, promises...);
}

/**
 * @brief 接收一个 promise 对象的集合，当其中的任意一个 promise
 * 成功，就返回那个成功的 promise 的值。
 * @tparam Promises Promise 类型。
 * @param promises Promise 集合。
 * @return Promise<int> 返回成功的 promise 的下标，若都失败返回 -1。
 */
template <typename... Promises>
inline Promise<int> any(Promises... promises) {
  Promise<int> result;
  auto pending_count = std::make_shared<size_t>(sizeof...(promises));
  anyUnpack(result, pending_count, 0, promises...);
  return result;
}

inline static void raceUnpack(Promise<size_t> result, size_t index) {}

template <typename P, typename... Promises>
inline void raceUnpack(Promise<size_t> result, size_t index, P first,
                       Promises... promises) {
  auto p = first.promise();
  if (p->settled()) {
    result.resolve(index);
    return;
  }

  auto on_settle = [result, index, first]() {
    auto p = first.promise();
    if (p->settled()) {
      return;
    }
    result.resolve(index);
  };
  p->appendOnSettle(std::move(on_settle));

  raceUnpack(result, index + 1, promises...);
}

/**
 * @brief 等到任意一个 promise 的状态变为已敲定。
 * @tparam Promises Promise 类型。
 * @param promises Promise 集合。
 * @return Promise<size_t> 敲定的 promise 的下标。
 */
template <typename... Promises>
inline Promise<size_t> race(Promises... promises) {
  Promise<size_t> result;
  raceUnpack(result, 0, promises...);
  return result;
}

#define ALL(...) switch (coro::all(__VA_ARGS__).await())
#define ALL_SETTLED(...) switch (coro::allSettled(__VA_ARGS__).await())
#define ANY(...) switch (coro::any(__VA_ARGS__).await())
#define RACE(...) switch (coro::race(__VA_ARGS__).await())

}  // namespace coro

#endif  // CORO_INCLUDE_CORO_PROMISE_HPP_
