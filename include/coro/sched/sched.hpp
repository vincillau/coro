#ifndef CORO_INCLUDE_CORO_SCHED_SCHED_HPP_
#define CORO_INCLUDE_CORO_SCHED_SCHED_HPP_

#include <boost/asio.hpp>
#include <memory>

#include "coro.hpp"

namespace coro {
namespace sched {

/**
 * @brief 获取当前线程的 Asio IO 上下文。
 * @return boost::asio::io_context& Asio IO 上下文。
 */
boost::asio::io_context& io_context();

/**
 * @brief 获取当前线程正在运行的协程对象。
 * @return std::shared_ptr<Coro> 正在运行的协程对象。
 */
std::shared_ptr<Coro> current();

/**
 * @brief 在当前线程中调度指定的协程。该协程将进入就绪态。
 * @param coro 协程对象。
 */
void schedule(std::shared_ptr<Coro> coro);

/**
 * @brief 如果当前线程中有就绪的协程，则让出 CPU。当前的协程将转为就绪态。
 */
void yield();

/**
 * @brief 阻塞当前协程。
 */
void block();

/**
 * @brief 唤醒指定的协程。
 * @param coro 协程对象。
 */
void wakeUp(std::shared_ptr<Coro> coro);

/**
 * @brief 退出当前协程。
 */
void exit();

/**
 * @brief 释放掉当前线程中已经终止的协程（如果有）。
 */
void freeDead();

}  // namespace sched
}  // namespace coro

#endif  // CORO_INCLUDE_CORO_SCHED_SCHED_HPP_
