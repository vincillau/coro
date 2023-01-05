#ifndef CORO_INCLUDE_CORO_TIMER_STEADY_TIMER_HPP_
#define CORO_INCLUDE_CORO_TIMER_STEADY_TIMER_HPP_

#include <boost/asio.hpp>
#include <chrono>

#include "coro/promise.hpp"

namespace coro {
namespace timer {

class SteadyTimer {
 public:
  SteadyTimer() : steady_timer_(sched::Sched::io_context()) {}

  Promise<void> expiresAt(
      std::chrono::time_point<std::chrono::steady_clock> expiry_time);
  Promise<void> expiresAfter(
      std::chrono::duration<int64_t, std::nano> expiry_time);

 private:
  boost::asio::steady_timer steady_timer_;
};

}  // namespace timer
}  // namespace coro

#endif  // CORO_INCLUDE_CORO_TIMER_STEADY_TIMER_HPP_
