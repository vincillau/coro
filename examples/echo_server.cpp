#include "coro/coro.hpp"

using coro::spawn;
using coro::tcp::Conn;
using coro::tcp::listen;

void handler(Conn conn) {
  for (;;) {
    char buf[4096];
    auto n = conn->readline(buf, 4096).await();
    if (n == 0) {
      conn->close();
      break;
    }
    conn->write(buf, n).await();
  }
}

int main() {
  auto listener = listen(8080);
  for (;;) {
    auto conn = listener->accept().await();
    spawn([conn]() { handler(conn); });
  }

  return 0;
}
