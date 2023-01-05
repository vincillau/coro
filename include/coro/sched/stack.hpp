/**
 * @file stack.hpp
 * @brief 协程栈的分配和释放，仅支持 x64。
 */

#ifndef CORO_INCLUDE_CORO_SCHED_STACK_HPP_
#define CORO_INCLUDE_CORO_SCHED_STACK_HPP_

#include <cstddef>

namespace coro {
namespace sched {

/**
 * @brief 分配指定大小的栈，返回指向栈底的指针。
 * @param size 栈的大小。
 * @return void* 指向栈底的指针。
 */
inline static void* allocStack(size_t size) {
  char* base = new char[size];
  return base + size;
}

/**
 * @brief 释放协程栈。
 * @param stack 由 allocStack 分配的栈内存。
 * @param size 栈大小。
 */
inline static void freeStack(void* stack, size_t size) {
  char* base = reinterpret_cast<char*>(stack) - size;
  delete[] base;
}

}  // namespace sched
}  // namespace coro

#endif  // CORO_INCLUDE_CORO_SCHED_STACK_HPP_
