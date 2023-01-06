#include "coro/sleep.hpp"

#include <boost/asio.hpp>
#include <chrono>
#include <memory>

#include "coro/sched/sched.hpp"

namespace coro {

Promise<void> milliSleep(size_t ms) {
  Promise<void> promise;
  auto timer = std::make_shared<boost::asio::steady_timer>(sched::io_context());
  timer->expires_after(std::chrono::milliseconds(ms));
  timer->async_wait([promise, timer](std::error_code error) {
    if (error) {
      promise.reject(std::move(error));
    } else {
      promise.resolve();
    }
  });
  return promise;
}

}  // namespace coro
