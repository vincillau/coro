#include "coro/sched/promise.hpp"

#include <gtest/gtest.h>

namespace coro {
namespace sched {

static constexpr size_t kStackSize = 1024 * 64;

TEST(PromiseTest, Resolve) {
  auto promise = std::make_shared<Promise<void>>();
  auto coro =
      std::make_shared<Coro>([promise]() { promise->resolve(); }, kStackSize);
  schedule(coro);
  yield();
  std::error_code error;
  promise->await(error);
  EXPECT_FALSE(error);
}

TEST(PromiseTest, ResolveWithValue) {
  auto promise = std::make_shared<Promise<int>>();
  auto coro =
      std::make_shared<Coro>([promise]() { promise->resolve(10); }, kStackSize);
  schedule(coro);
  yield();
  std::error_code error;
  int value = promise->await(error);
  EXPECT_FALSE(error);
  EXPECT_EQ(value, 10);
}

TEST(PromiseTest, Reject) {
  auto promise = std::make_shared<Promise<void>>();
  auto coro = std::make_shared<Coro>(
      [promise]() {
        std::error_code error(static_cast<int>(std::errc::invalid_argument),
                              std::system_category());
        promise->reject(std::move(error));
      },
      kStackSize);
  schedule(coro);
  yield();
  std::error_code error;
  std::error_code want(static_cast<int>(std::errc::invalid_argument),
                       std::system_category());
  promise->await(error);
  EXPECT_EQ(error, want);
}

TEST(PromiseTest, RejectIntType) {
  auto promise = std::make_shared<Promise<int>>();
  auto coro = std::make_shared<Coro>(
      [promise]() {
        std::error_code error(static_cast<int>(std::errc::invalid_argument),
                              std::system_category());
        promise->reject(std::move(error));
      },
      kStackSize);
  schedule(coro);
  yield();
  std::error_code error;
  std::error_code want(static_cast<int>(std::errc::invalid_argument),
                       std::system_category());
  promise->await(error);
  EXPECT_EQ(error, want);
}

TEST(PromiseTest, Then) {
  auto promise = std::make_shared<Promise<int>>();
  auto coro =
      std::make_shared<Coro>([promise]() { promise->resolve(10); }, kStackSize);
  schedule(coro);
  int val = 0;
  promise->then([&val](int value) { val = value; });
  promise->except([](std::error_code error) { FAIL(); });
  yield();
  EXPECT_EQ(val, 10);
}

TEST(PromiseTest, Except) {
  auto promise = std::make_shared<Promise<int>>();
  auto coro = std::make_shared<Coro>(
      [promise]() {
        std::error_code error(static_cast<int>(std::errc::invalid_argument),
                              std::system_category());
        promise->reject(std::move(error));
      },
      kStackSize);
  schedule(coro);
  std::error_code err;
  promise->then([](int value) { FAIL(); });
  promise->except([&err](std::error_code error) { err = error; });
  yield();
  std::error_code want(static_cast<int>(std::errc::invalid_argument),
                       std::system_category());
  EXPECT_EQ(err, want);
}

TEST(PromiseTest, Finally) {
  auto promise = std::make_shared<Promise<void>>();
  auto coro =
      std::make_shared<Coro>([promise]() { promise->resolve(); }, kStackSize);
  schedule(coro);
  int val = 0;
  promise->finally([&val]() { val = 10; });
  yield();
  EXPECT_EQ(val, 10);
}

}  // namespace sched
}  // namespace coro
