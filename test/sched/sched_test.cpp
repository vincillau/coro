#include "coro/sched/sched.hpp"

#include <gtest/gtest.h>

namespace coro {
namespace sched {

static constexpr size_t kStackSize = 64 * 1024;

TEST(CoroTest, Current) {
  std::shared_ptr<Coro> cur;
  auto coro = std::make_shared<Coro>([&cur]() { cur = current(); }, kStackSize);
  schedule(coro);
  yield();
  EXPECT_EQ(cur.get(), coro.get());
}

TEST(CoroTest, BlockAndWakeUp) {
  auto cur = current();
  auto coro = std::make_shared<Coro>([cur]() { wakeUp(cur); }, kStackSize);
  schedule(coro);
  block();
  SUCCEED();
}

class SetOnDestory {
 public:
  SetOnDestory(int* val) : val_(val) {}
  ~SetOnDestory() { *val_ = 1; }

 private:
  int* val_;
};

TEST(CoroTest, Exit) {
  int val = 0;
  auto set_on_destory = std::make_shared<SetOnDestory>(&val);
  // 协程退出后协程函数被析构，lambda 表达式持有的智能指针也应该被析构
  auto coro = std::make_shared<Coro>([set_on_destory]() {}, kStackSize);
  schedule(coro);
  coro.reset();
  set_on_destory.reset();

  ASSERT_EQ(val, 0);
  yield();
  EXPECT_EQ(val, 1);
}

}  // namespace sched
}  // namespace coro
