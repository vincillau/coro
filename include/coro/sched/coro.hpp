#ifndef CORO_INCLUDE_CORO_SCHED_CORO_HPP_
#define CORO_INCLUDE_CORO_SCHED_CORO_HPP_

#include <cstddef>
#include <functional>
#include <memory>

#include "fctx.hpp"

namespace coro {
namespace sched {

class Coro {
 public:
  using Handle = std::shared_ptr<Coro>;
  using Func = std::function<void()>;

  Coro() = default;
  Coro(Func func, size_t stack_size);
  Coro(const Coro&) = delete;
  Coro& operator=(const Coro&) = delete;
  Coro(Coro&&) = delete;
  Coro& operator=(Coro&&) = delete;
  ~Coro();

  void resume(Coro* prev) const;

 private:
  static void fctxFunc(transfer_t trans);

  Func func_;
  fcontext_t fctx_ = nullptr;
  void* stack_ = nullptr;
  size_t stack_size_ = 0;
};

}  // namespace sched
}  // namespace coro

#endif  // CORO_INCLUDE_CORO_SCHED_CORO_HPP_
