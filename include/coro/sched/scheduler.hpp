/**
 * @file scheduler.hpp
 * @brief 协程调度器。
 */

#ifndef CORO_INCLUDE_CORO_SCHED_SCHEDULER_HPP_
#define CORO_INCLUDE_CORO_SCHED_SCHEDULER_HPP_

#include <boost/asio.hpp>
#include <memory>
#include <queue>

#include "coro.hpp"

namespace coro {
namespace sched {

/**
 * @brief 协程调度器，每线程一个。
 * 调度器中的协程有三种状态：运行、就绪和阻塞。
 * 每个调度器（线程）中有且只有一个协程处于运行状态。
 * 当协程等待 Promise 时，如果该 Promise 尚未敲定，则协程会放弃 CPU
 * 从运行状态转换为阻塞状态。当 Promise 敲定时协程会被唤醒，等待调度器调度。
 * 处于运行状态的协程也可以调用 yield() 主动让出 CPU 进入就绪状态。
 * 每个调度器都有一个 idle 协程。当调度器中没有协程处于运行或就绪状态，
 * 则 idle 会被调度。idle 协程会调用 io_context::run_one()，等待 IO 任务完成。
 */
class Scheduler {
 public:
  Scheduler();
  // 禁止拷贝和移动，Scheduler 由 std::unique_ptr 持有。
  Scheduler(const Scheduler&) = delete;
  Scheduler& operator=(const Scheduler&) = delete;
  Scheduler(Scheduler&&) = delete;
  Scheduler& operator=(Scheduler&&) = delete;
  ~Scheduler();

  boost::asio::io_context& io_context() { return io_context_; }
  std::shared_ptr<Coro> current() const { return current_; }

  /**
   * @brief 调度指定的协程。该协程将进入就绪态。
   * @param coro 协程对象。
   */
  void schedule(std::shared_ptr<Coro> coro);

  /**
   * @brief 如果当前调度器中有就绪的协程，则让出 CPU。当前的协程将转为就绪态。
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

 private:
  /**
   * @brief idle 协程执行的函数。
   *
   */
  void idleFunc();

  boost::asio::io_context io_context_;  // Asio IO 上下文。
  std::shared_ptr<Coro> current_;       // 当前正在运行的协程。
  std::shared_ptr<Coro> idle_;          // 空闲协程。
  // 退出的协程，将会被下一个协程清理。
  std::shared_ptr<Coro> dead_;
  // 就绪协程队列，按先进先出的顺序被调度。
  std::queue<std::shared_ptr<Coro>> ready_queue_;
  size_t blocked_count_ = 0;  // 处于阻塞状态的协程数量。
};

}  // namespace sched
}  // namespace coro

#endif  // CORO_INCLUDE_CORO_SCHED_SCHEDULER_HPP_
