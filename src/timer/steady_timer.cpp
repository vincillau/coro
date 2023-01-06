#include "coro/timer/steady_timer.hpp"

namespace coro {
namespace timer {

Promise<void> SteadyTimer::expiresAt(
    std::chrono::time_point<std::chrono::steady_clock> expiry_time) {
  Promise<void> promise;
  boost::system::error_code error;
  steady_timer_.expires_at(expiry_time, error);
  if (error) {
    promise.reject(error);
    return promise;
  }
  steady_timer_.async_wait([promise](std::error_code error) {
    if (error) {
      promise.reject(std::move(error));
    } else {
      promise.resolve();
    }
  });
  return promise;
}

Promise<void> SteadyTimer::expiresAfter(
    std::chrono::duration<int64_t, std::nano> expiry_time) {
  Promise<void> promise;
  steady_timer_.expires_after(expiry_time);
  steady_timer_.async_wait([promise](std::error_code error) {
    if (error) {
      promise.reject(std::move(error));
    } else {
      promise.resolve();
    }
  });
  return promise;
}

}  // namespace timer
}  // namespace coro
