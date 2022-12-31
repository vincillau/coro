#ifndef CORO_INCLUDE_CORO_TIMER_SYSTEM_TIMER_HPP_
#define CORO_INCLUDE_CORO_TIMER_SYSTEM_TIMER_HPP_

#include <boost/asio.hpp>
#include <chrono>

#include "coro/promise.hpp"

namespace coro {
namespace timer {

class SystemTimer {
 public:
  SystemTimer() : system_timer_(sched::Sched::io_context()) {}

  Promise<void> expiresAt(
      std::chrono::time_point<std::chrono::system_clock> expiry_time);
  Promise<void> expiresAfter(std::chrono::duration<int64_t> expiry_time);

 private:
  boost::asio::system_timer system_timer_;
};

}  // namespace timer
}  // namespace coro

#endif  // CORO_INCLUDE_CORO_TIMER_SYSTEM_TIMER_HPP_
