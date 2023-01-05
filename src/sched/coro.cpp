#include "coro/sched/coro.hpp"

#include <iostream>
#include <utility>

#include "coro/sched/fctx.hpp"
#include "coro/sched/sched.hpp"
#include "coro/sched/stack.hpp"

namespace coro {
namespace sched {

void Coro::funcWrapper(transfer_t trans) {
  auto prev = static_cast<Coro*>(trans.data);
  // 更新上一个协程对象的 fcontext。
  // 因为主协程（线程的第一个协程）的 fctx_ 为 nullptr。
  prev->fctx_ = trans.fctx;
  freeDead();

  try {
    // 使用代码块确保 cur 智能指针被析构。
    {
      auto cur = current();
      cur->func_();
    }
    // 退出当前协程，协程对象由下一个协程析构。
    exit();
  }
  // 协程函数不允许抛出异常，异常必须在协程函数内部被捕获。
  catch (const std::exception& e) {
    std::cerr << "The coroutine function throws an uncaught exception: "
              << e.what() << std::endl;
    std::terminate();
  } catch (...) {
    std::cerr << "The coroutine function throws an uncaught exception."
              << std::endl;
    std::terminate();
  }
}

Coro::Coro(Func func, size_t stack_size)
    : func_(std::move(func)), stack_size_(stack_size) {
  void* stack = allocStack(stack_size);
  fctx_ = make_fcontext(stack, stack_size, funcWrapper);
}

Coro::~Coro() {
  if (stack_) {
    freeStack(stack_, stack_size_);
  }
}

void Coro::resume(Coro* prev) const {
  transfer_t trans = jump_fcontext(fctx_, prev);
  // 协程只会在两种情况下被恢复：
  // 1. make_fcontext 传入的函数指针第一次被执行；
  // 2. jump_fcontext 返回。
  // 协程被恢复后都要进行以下两个操作：
  // 1. 更新前一个 Coro 对象的 fctx_；
  // 2. 调用 freeDead()，如果前一个协程已经退出，freeDead() 会将其析构。
  prev = static_cast<Coro*>(trans.data);
  prev->fctx_ = trans.fctx;
  freeDead();
}

}  // namespace sched
}  // namespace coro
