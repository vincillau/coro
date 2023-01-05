#include "coro/promise.hpp"

#include <gtest/gtest.h>

namespace coro {

TEST(AllTest, ResolveAll) {
  Promise<void> p1, p2, p3;
  p1.resolve();
  p2.resolve();
  p3.resolve();
  auto p = all(p1, p2, p3);
  int index = p.await();
  EXPECT_EQ(index, -1);
}

TEST(AllTest, RejectedOne) {
  Promise<void> p1, p2, p3;
  p1.resolve();
  std::error_code error(static_cast<int>(std::errc::invalid_argument),
                        std::system_category());
  p2.reject(error);
  p3.resolve();
  auto p = all(p1, p2, p3);
  int index = p.await();
  EXPECT_EQ(index, 1);
}

TEST(AllSettledTest, ResolveAll) {
  Promise<void> p1, p2, p3;
  p1.resolve();
  p2.resolve();
  p3.resolve();
  auto p = allSettled(p1, p2, p3);
  p.await();
  SUCCEED();
}

TEST(AllSettledTest, RejectAll) {
  Promise<void> p1, p2, p3;
  std::error_code error(static_cast<int>(std::errc::invalid_argument),
                        std::system_category());
  p1.reject(error);
  p2.reject(error);
  p3.reject(error);
  auto p = allSettled(p1, p2, p3);
  p.await();
  SUCCEED();
}

TEST(AllSettledTest, RejectOne) {
  Promise<void> p1, p2, p3;
  p1.resolve();
  std::error_code error(static_cast<int>(std::errc::invalid_argument),
                        std::system_category());
  p2.reject(error);
  p3.resolve();
  auto p = allSettled(p1, p2, p3);
  p.await();
  SUCCEED();
}

TEST(AnyTest, Resolve) {
  Promise<void> p1, p2, p3;
  p3.resolve();
  auto p = any(p1, p2, p3);
  int index = p.await();
  EXPECT_EQ(index, 2);

  p1.resolve();
  p2.resolve();
}

TEST(AnyTest, RejectAll) {
  Promise<void> p1, p2, p3;
  std::error_code error(static_cast<int>(std::errc::invalid_argument),
                        std::system_category());
  p1.reject(error);
  p2.reject(error);
  p3.reject(error);
  auto p = any(p1, p2, p3);
  int index = p.await();
  EXPECT_EQ(index, -1);
}

TEST(RaceTest, ResolveOne) {
  Promise<void> p1, p2, p3;
  p2.resolve();
  auto p = race(p1, p2, p3);
  int index = p.await();
  EXPECT_EQ(index, 1);

  p1.resolve();
  p3.resolve();
}

TEST(RaceTest, RejectOne) {
  Promise<void> p1, p2, p3;
  std::error_code error(static_cast<int>(std::errc::invalid_argument),
                        std::system_category());
  p3.reject(error);
  auto p = race(p1, p2, p3);
  int index = p.await();
  EXPECT_EQ(index, 2);

  p1.resolve();
  p2.resolve();
}

}  // namespace coro
