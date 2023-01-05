#include "coro/spawn.hpp"

#include <gtest/gtest.h>

namespace coro {

TEST(SpawnTest, RerurnVoid) {
  int val = 0;
  auto promise = spawn([&val]() { val = 1; });
  promise.await();
  EXPECT_EQ(val, 1);
}

TEST(SpawnTest, RerurnInt) {
  auto promise = spawn([]() { return 10; });
  int val = promise.await();
  EXPECT_EQ(val, 10);
}

}  // namespace coro
