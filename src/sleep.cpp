#include "coro/sleep.hpp"

#include <boost/asio.hpp>
#include <chrono>
#include <memory>
#include <system_error>

#include "coro/sched/promise.hpp"
#include "coro/sched/sched.hpp"

namespace coro {

Promise<void> sleep(size_t ms) {
  Promise<void> promise;
  auto weak_promise = promise.weak();
  auto timer =
      std::make_shared<boost::asio::steady_timer>(sched::Sched::io_context());
  timer->expires_after(std::chrono::milliseconds(ms));
  timer->async_wait([weak_promise](std::error_code error) {
    auto shared_promise = weak_promise.lock();
    if (shared_promise) {
      if (error) {
        shared_promise->reject(std::move(error));
      } else {
        shared_promise->resolve();
      }
    }
  });
  promise.shared()->pushOnDestory([timer]() {});
  return promise;
}

}  // namespace coro
