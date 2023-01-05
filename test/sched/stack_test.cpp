#include "coro/sched/stack.hpp"

#include <gtest/gtest.h>

namespace coro {
namespace sched {

TEST(StackTest, AllocStack) {
  constexpr size_t stack_size = 4096;
  auto stack = allocStack(stack_size);
  EXPECT_NE(stack, nullptr);
}

TEST(StackTest, FreeStack) {
  constexpr size_t stack_size = 4096;
  auto stack = allocStack(stack_size);
  EXPECT_NE(stack, nullptr);
  freeStack(stack, stack_size);
}

}  // namespace sched
}  // namespace coro
