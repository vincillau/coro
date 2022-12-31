#ifndef CORO_INCLUDE_CORO_SCHED_STACK_HPP_
#define CORO_INCLUDE_CORO_SCHED_STACK_HPP_

#include <cstddef>

namespace coro {
namespace sched {

inline static void* allocStack(size_t size) {
  char* base = new char[size];
  return base + size;
}

inline static void freeStack(void* stack, size_t size) {
  char* base = reinterpret_cast<char*>(stack) - size;
  delete[] base;
}

}  // namespace sched
}  // namespace coro

#endif  // CORO_INCLUDE_CORO_SCHED_STACK_HPP_
