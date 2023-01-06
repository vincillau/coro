/**
 * @file steady_timer.hpp
 * @brief 基于稳定时钟的定时器。
 */

#ifndef CORO_INCLUDE_CORO_TIMER_STEADY_TIMER_HPP_
#define CORO_INCLUDE_CORO_TIMER_STEADY_TIMER_HPP_

#include <boost/asio.hpp>
#include <chrono>

#include "coro/promise.hpp"
#include "coro/sched/sched.hpp"

namespace coro {
namespace timer {

class SteadyTimer {
 public:
  SteadyTimer() : steady_timer_(sched::io_context()) {}

  /**
   * @brief 在指定时间点过期，上一次过期任务会被取消。
   * @param expiry_time 过期时间点。
   * @return Promise<void> 在过期时兑现的 Promise，可能出错。
   */
  Promise<void> expiresAt(
      std::chrono::time_point<std::chrono::steady_clock> expiry_time);

  /**
   * @brief 在指定时长后过期，上一次过期任务会被取消。
   * @param expiry_time 现在到过期时间的时长。
   * @return Promise<void> 在过期时兑现的 Promise，可能出错。
   */
  Promise<void> expiresAfter(
      std::chrono::duration<int64_t, std::nano> expiry_time);

 private:
  boost::asio::steady_timer steady_timer_;
};

}  // namespace timer
}  // namespace coro

#endif  // CORO_INCLUDE_CORO_TIMER_STEADY_TIMER_HPP_
