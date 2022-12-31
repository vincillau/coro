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

boost::asio::io_context& Sched::io_context() {
  return threadSched->io_context_;
}

void Sched::add(Coro::Handle coro) { threadSched->ready_coros_.push(coro); }

void Sched::yield() {
  if (threadSched->ready_coros_.empty()) {
    return;
  }
  auto prev = threadSched->current_.get();
  threadSched->ready_coros_.push(threadSched->current_);
  threadSched->current_ = threadSched->ready_coros_.front();
  threadSched->ready_coros_.pop();
  threadSched->current_->resume(prev);
}

void Sched::block() {
  auto prev = threadSched->current_.get();
  if (threadSched->ready_coros_.empty()) {
    threadSched->current_ = threadSched->idle_;
  } else {
    threadSched->current_ = threadSched->ready_coros_.front();
    threadSched->ready_coros_.pop();
  }
  threadSched->current_->resume(prev);
}

void Sched::exit() {
  threadSched->dead_ = threadSched->current_;
  auto prev = threadSched->current_.get();
  if (threadSched->ready_coros_.empty()) {
    threadSched->current_ = threadSched->idle_;
  } else {
    threadSched->current_ = threadSched->ready_coros_.front();
    threadSched->ready_coros_.pop();
  }
  threadSched->current_->resume(prev);
}

void Sched::cleanDead() { threadSched->dead_.reset(); }

void Sched::idleFunc() {
  auto work_guard = boost::asio::make_work_guard(threadSched->io_context());
  while (true) {
    threadSched->io_context_.run_one();
    if (threadSched->ready_coros_.empty()) {
      continue;
    }
    auto prev = threadSched->idle_.get();
    threadSched->current_ = threadSched->ready_coros_.front();
    threadSched->ready_coros_.pop();
    threadSched->current_->resume(prev);
  }
}

}  // namespace sched
}  // namespace coro
