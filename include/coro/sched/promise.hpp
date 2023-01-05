/**
 * @file promise.hpp
 * @brief promise 实现。
 */

#ifndef CORO_INCLUDE_CORO_SCHED_PROMISE_HPP_
#define CORO_INCLUDE_CORO_SCHED_PROMISE_HPP_

#include <cassert>
#include <functional>
#include <system_error>
#include <vector>

#include "sched.hpp"

namespace coro {
namespace sched {

/**
 * @brief Promise 是表示一个异步函数的返回结果。
 * @tparam T Promise 值的类型。
 */
template <typename T>
class Promise {
 public:
  /**
   * @brief 构造一个 Promise 对象，该 Promise 对象处于未决状态。
   */
  Promise() = default;

  // Promise 对象禁止拷贝和移动，所有 Promise 对象均由 std::shared_ptr 持有。
  Promise(const Promise&) = delete;
  bool operator=(const Promise&) = delete;
  Promise(Promise&&) = delete;
  bool operator=(Promise&&) = delete;

  /**
   * @brief 销毁 Promise 对象，销毁前将调用 onDestory_ 回调。
   */
  ~Promise();

  /**
   * @brief 将 Promise 设置为已解决，并将 value 设置为异步函数的执行结果。
   * 调用此函数前 Promise 必须处于未决状态。
   * @param value 异步函数的执行结果。
   */
  void resolve(T value);

  /**
   * @brief 将 Promise 设置为已拒绝，并指定错误码。
   * 调用此函数前 Promise 必须处于未决状态。
   * @param error 错误码。
   */
  void reject(std::error_code error);

  /**
   * @brief 如果 Promise 未敲定，则阻塞当前协程直至 Promise
   * 敲定，返回异步函数执行结果。Promise 只能调用 await 一次。
   * @param error Promise 的错误码。
   * @return T Promise 的执行结果，如果 Promise
   * 被设置为已拒绝，返回值将没有意义。
   */
  T await(std::error_code& error);

  /**
   * @brief Promise 被解决时调用指定回调。
   * @param callback 回调函数，value 为异步函数执行结果。
   */
  void then(std::function<void(T value)> callback);

  /**
   * @brief Promise 被拒绝时调用指定回调。
   * @param callback 回调函数，error 为错误码。
   */
  void except(std::function<void(std::error_code error)> callback);

  /**
   * @brief Promise 被敲定时调用指定回调。
   * @param callback 回调函数。
   */
  void finally(std::function<void()> callback);

  /**
   * @brief 判断 Promise 是否已敲定。
   * @return true 已敲定。
   * @return false 未敲定。
   */
  bool settled() const { return settled_; }

  /**
   * @brief 判断 Promise 是否处于已拒绝状态，只有 Promise
   * 已敲定，返回值才有意义。
   * @return true 已拒绝。
   * @return false 未决或已解决。
   */
  bool err() const { return error_.value() != 0; }

  /**
   * @brief 追加在敲定时调用的回调。
   * @param callback
   */
  void appendOnSettle(std::function<void()> callback) {
    on_settle_.emplace_back(std::move(callback));
  }

 private:
  T value_;                                       // Promise 的结果。
  std::error_code error_;                         // 错误码。
  bool settled_ = false;                          // Promise 是否已敲定。
  std::vector<std::function<void()>> on_settle_;  // 敲定时执行的回调。
  std::function<void()> on_destory_;              // 销毁时执行的回调。
};

template <typename T>
inline Promise<T>::~Promise() {
  assert(settled());
  if (on_destory_) {
    on_destory_();
  }
}

template <typename T>
inline void Promise<T>::resolve(T value) {
  assert(!settled());
  settled_ = true;
  value_ = std::move(value);
  for (const std::function<void()>& cb : on_settle_) {
    cb();
  }
}

template <typename T>
inline void Promise<T>::reject(std::error_code error) {
  assert(!settled());
  settled_ = true;
  error_ = std::move(error);
  for (const std::function<void()>& cb : on_settle_) {
    cb();
  }
}

template <typename T>
inline T Promise<T>::await(std::error_code& error) {
  if (settled()) {
    error = std::move(error_);
    return std::move(value_);
  }
  auto coro = current();
  appendOnSettle([coro]() { wakeUp(coro); });
  block();
  error = std::move(error_);
  return std::move(value_);
}

template <typename T>
inline void Promise<T>::then(std::function<void(T value)> callback) {
  appendOnSettle([this, callback]() {
    if (!err()) {
      callback(std::move(value_));
    }
  });
}

template <typename T>
inline void Promise<T>::except(
    std::function<void(std::error_code error)> callback) {
  appendOnSettle([this, callback]() {
    if (err()) {
      callback(std::move(error_));
    }
  });
}

template <typename T>
inline void Promise<T>::finally(std::function<void()> callback) {
  appendOnSettle([this, callback]() { callback(); });
}

/**
 * @brief Promise 的 void 特化。
 */
template <>
class Promise<void> {
 public:
  Promise() = default;
  Promise(const Promise&) = delete;
  bool operator=(const Promise&) = delete;
  Promise(Promise&&) = delete;
  bool operator=(Promise&&) = delete;
  ~Promise();

  void resolve();
  void reject(std::error_code error);

  void await(std::error_code& error);

  void then(std::function<void()> callback);
  void except(std::function<void(std::error_code error)> callback);
  void finally(std::function<void()> callback);

  bool settled() const { return settled_; }
  bool err() const { return error_.value() != 0; }

  void appendOnSettle(std::function<void()> callback) {
    on_settle_.emplace_back(std::move(callback));
  }

 private:
  std::error_code error_;
  bool settled_ = false;
  std::vector<std::function<void()>> on_settle_;
  std::function<void()> on_destory_;
};

inline Promise<void>::~Promise() {
  assert(settled());
  if (on_destory_) {
    on_destory_();
  }
}

inline void Promise<void>::resolve() {
  assert(!settled());
  settled_ = true;
  for (const std::function<void()>& cb : on_settle_) {
    cb();
  }
}

inline void Promise<void>::reject(std::error_code error) {
  assert(!settled());
  settled_ = true;
  error_ = std::move(error);
  for (const std::function<void()>& cb : on_settle_) {
    cb();
  }
}

inline void Promise<void>::await(std::error_code& error) {
  if (settled()) {
    error = std::move(error_);
    return;
  }
  auto coro = current();
  appendOnSettle([coro]() { wakeUp(coro); });
  block();
  error = std::move(error_);
}

inline void Promise<void>::then(std::function<void()> callback) {
  appendOnSettle([this, callback]() {
    if (!err()) {
      callback();
    }
  });
}

inline void Promise<void>::except(
    std::function<void(std::error_code error)> callback) {
  appendOnSettle([this, callback]() {
    if (err()) {
      callback(std::move(error_));
    }
  });
}

inline void Promise<void>::finally(std::function<void()> callback) {
  appendOnSettle([this, callback]() { callback(); });
}

}  // namespace sched
}  // namespace coro

#endif  // CORO_INCLUDE_CORO_SCHED_PROMISE_HPP_
