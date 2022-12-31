#include "coro/sched/sched.hpp"

namespace coro {
namespace sched {

static thread_local std::unique_ptr<Sched> threadSched =
    std::unique_ptr<Sched>(new Sched());

static constexpr size_t kIdleCoroStackSize = 1024 * 64;

Sched::Sched()
    : current_(std::make_shared<Coro>()),
      idle_(std::make_shared<Coro>(idleFunc, kIdleCoroStackSize)) {}

Coro::Handle Sched::current() { return threadSched->current_; }

void Sched::add(Coro::Handle coro) { threadSched->ready_coros_.push(coro); }

void Sched::yield() {
  if (threadSched->ready_coros_.empty()) {
    return;
  }
  auto prev = threadSched->current_;
  threadSched->ready_coros_.push(prev);
  auto next = threadSched->ready_coros_.front();
  threadSched->ready_coros_.pop();
  threadSched->current_ = next;
  next->resume(prev.get());
}

void Sched::block() {
  auto prev = threadSched->current_;
  Coro::Handle next;
  if (threadSched->ready_coros_.empty()) {
    next = threadSched->idle_;
  } else {
    next = threadSched->ready_coros_.front();
    threadSched->ready_coros_.pop();
  }
  threadSched->current_ = next;
  next->resume(prev.get());
}

void Sched::exit() {
  auto prev = threadSched->current_;
  Coro::Handle next;
  if (threadSched->ready_coros_.empty()) {
    next = threadSched->idle_;
  } else {
    next = threadSched->ready_coros_.front();
    threadSched->ready_coros_.pop();
  }
  threadSched->dead_ = prev;
  threadSched->current_ = next;
  next->resume(prev.get());
}

void Sched::cleanDead() { threadSched->dead_.reset(); }

void Sched::idleFunc() {
  while (true) {
    threadSched->io_context_.run_one();
    if (threadSched->ready_coros_.empty()) {
      continue;
    }
    auto prev = threadSched->idle_;
    auto next = threadSched->ready_coros_.front();
    threadSched->ready_coros_.pop();
    threadSched->current_ = next;
    next->resume(prev.get());
  }
}

}  // namespace sched
}  // namespace coro
