#include "coro/sched/scheduler.hpp"

#include <cassert>
#include <iostream>
#include <utility>

namespace coro {
namespace sched {

static constexpr size_t kIdleCoroStackSize = 1024 * 64;

Scheduler::Scheduler()
    : current_(std::make_shared<Coro>()),
      idle_(std::make_shared<Coro>([this]() { idleFunc(); },
                                   kIdleCoroStackSize)) {}

Scheduler::~Scheduler() {
  if (!ready_queue_.empty() || blocked_count_) {
    std::cerr << "A thread can only exit when only the main coroutine is alive."
              << std::endl;
    std::terminate();
  }
}

void Scheduler::schedule(std::shared_ptr<Coro> coro) {
  ready_queue_.push(coro);
}

void Scheduler::yield() {
  if (ready_queue_.empty()) {
    return;
  }
  ready_queue_.push(current_);
  auto prev = current_.get();
  current_ = ready_queue_.front();
  ready_queue_.pop();
  current_->resume(prev);
}

void Scheduler::block() {
  blocked_count_++;
  auto prev = current_.get();
  if (ready_queue_.empty()) {
    current_ = idle_;
  } else {
    current_ = ready_queue_.front();
    ready_queue_.pop();
  }
  current_->resume(prev);
}

void Scheduler::wakeUp(std::shared_ptr<Coro> coro) {
  assert(blocked_count_ > 0);
  blocked_count_--;
  ready_queue_.push(coro);
}

void Scheduler::exit() {
  dead_ = current_;
  auto prev = current_.get();
  if (ready_queue_.empty()) {
    current_ = idle_;
  } else {
    current_ = ready_queue_.front();
    ready_queue_.pop();
  }
  current_->resume(prev);
}

void Scheduler::freeDead() { dead_.reset(); }

void Scheduler::idleFunc() {
  for (;;) {
    assert(current_ == idle_);
    if (!ready_queue_.empty()) {
      auto prev = current_.get();
      current_ = ready_queue_.front();
      ready_queue_.pop();
      current_->resume(prev);
    }
    assert(current_ == idle_);
    io_context_.run_one();
  }
}

}  // namespace sched
}  // namespace coro
