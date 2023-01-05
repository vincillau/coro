#ifndef CORO_INCLUDE_CORO_SLEEP_HPP_
#define CORO_INCLUDE_CORO_SLEEP_HPP_

#include "promise.hpp"
#include "timer/steady_timer.hpp"

namespace coro {

Promise<void> sleep(size_t ms);

}  // namespace coro

#endif  // CORO_INCLUDE_CORO_SLEEP_HPP_
