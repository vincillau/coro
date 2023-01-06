#ifndef CORO_INCLUDE_CORO_SLEEP_HPP_
#define CORO_INCLUDE_CORO_SLEEP_HPP_

#include <cstddef>

#include "coro/promise.hpp"

namespace coro {

/**
 * @brief 创建一个 ms 毫秒后兑现的 Promise。
 * @param ms 时长，单位毫秒。
 * @return Promise<void> ms 毫秒后兑现的 Promise。
 */
Promise<void> milliSleep(size_t ms);

/**
 * @brief 创建一个 s 秒后兑现的 Promise。
 * @param s 时长，单位秒。
 * @return Promise<void> s 秒后兑现的 Promise。
 */
inline Promise<void> sleep(size_t s) { return milliSleep(s * 1000); }

}  // namespace coro

#endif  // CORO_INCLUDE_CORO_SLEEP_HPP_
