#include "coro/sched/sched.hpp"

#include "coro/sched/scheduler.hpp"

namespace coro {
namespace sched {

static thread_local auto scheduler =
    std::unique_ptr<Scheduler>(new Scheduler());

boost::asio::io_context& io_context() { return scheduler->io_context(); }

std::shared_ptr<Coro> current() { return scheduler->current(); }

void schedule(std::shared_ptr<Coro> coro) { scheduler->schedule(coro); }

void yield() { scheduler->yield(); }

void block() { scheduler->block(); }

void wakeUp(std::shared_ptr<Coro> coro) { scheduler->wakeUp(coro); }

void exit() { scheduler->exit(); }

void freeDead() { scheduler->freeDead(); }

}  // namespace sched
}  // namespace coro
