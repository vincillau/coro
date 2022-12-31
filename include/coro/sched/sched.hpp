#ifndef CORO_INCLUDE_CORO_SCHED_SCHED_HPP_
#define CORO_INCLUDE_CORO_SCHED_SCHED_HPP_

#include <boost/asio.hpp>
#include <memory>
#include <queue>

#include "coro.hpp"

namespace coro {
namespace sched {

class Sched {
 public:
  Sched();

  static Coro::Handle current();
  static boost::asio::io_context& io_context();

  static void add(Coro::Handle coro);

  static void yield();
  static void block();
  static void exit();
  static void cleanDead();

 private:
  static void init();
  static void idleFunc();

  boost::asio::io_context io_context_;
  Coro::Handle idle_;
  Coro::Handle current_;
  Coro::Handle dead_;
  std::queue<Coro::Handle> ready_coros_;
};

}  // namespace sched
}  // namespace coro

#endif  // CORO_INCLUDE_CORO_SCHED_SCHED_HPP_
