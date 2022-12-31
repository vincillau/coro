#include "coro/sched/coro.hpp"

#include <iostream>
#include <utility>

#include "coro/sched/fctx.hpp"
#include "coro/sched/sched.hpp"
#include "coro/sched/stack.hpp"

namespace coro {
namespace sched {

void Coro::fctxFunc(transfer_t trans) {
  auto prev = static_cast<Coro*>(trans.data);
  prev->fctx_ = trans.fctx;
  try {
    sched::Sched::current()->func_();
    Sched::exit();
  } catch (...) {
    std::cerr << "the coroutine function throws an uncaught exception"
              << std::endl;
    std::terminate();
  }
}

Coro::Coro(Func func, size_t stack_size)
    : func_(std::move(func)), stack_size_(stack_size) {
  void* stack = allocStack(stack_size);
  fctx_ = make_fcontext(stack, stack_size, fctxFunc);
}

Coro::~Coro() {
  if (stack_) {
    freeStack(stack_, stack_size_);
  }
}

void Coro::resume(Coro* prev) const {
  transfer_t trans = jump_fcontext(fctx_, prev);
  prev = static_cast<Coro*>(trans.data);
  prev->fctx_ = trans.fctx;
  Sched::cleanDead();
}

}  // namespace sched
}  // namespace coro
