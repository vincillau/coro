#include "coro/timer/system_timer.hpp"

namespace coro {
namespace timer {

Promise<void> SystemTimer::expiresAt(
    std::chrono::time_point<std::chrono::system_clock> expiry_time) {
  Promise<void> promise;
  boost::system::error_code error;
  system_timer_.expires_at(expiry_time, error);
  if (error) {
    promise.reject(error);
    return promise;
  }
  system_timer_.async_wait([promise](std::error_code error) {
    if (error) {
      promise.reject(std::move(error));
    } else {
      promise.resolve();
    }
  });
  return promise;
}

Promise<void> SystemTimer::expiresAfter(
    std::chrono::duration<int64_t, std::nano> expiry_time) {
  Promise<void> promise;
  system_timer_.expires_after(expiry_time);
  system_timer_.async_wait([promise](std::error_code error) {
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
