#ifndef CORO_INCLUDE_CORO_SCHED_HPP_
#define CORO_INCLUDE_CORO_SCHED_HPP_

#include "sched/sched.hpp"

namespace coro {

/**
 * @brief 让出协程。
 */
inline static void yield() { sched::yield(); }

}  // namespace coro

#endif  // CORO_INCLUDE_CORO_SCHED_HPP_
