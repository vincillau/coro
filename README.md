# Coro

C++11 协程库，支持 TCP、HTTP 和 Redis 协议。

## 构建

Coro 使用 [xmake](https://xmake.io) 构建项目和管理依赖：

```bash
git clone https://github.com/vincillau/coro.git
cd coro
xmake build
```

运行用例：

```bash
xmake run echo-server
xmake run http-client
xmake run redis-client
xmake run http-server
```

## Hello World

使用 Coro 实现 echo server:

```c++
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
  // 创建一个监听 8080 端口的 TCP listener
  auto listener = listen(8080);
  for (;;) {
    // 接受一个 TCP 连接，await 会阻塞当前协程直至连接完成
    auto conn = listener->accept().await();
    // 创建一个新的协程
    spawn([conn]() { handler(conn); });
  }

  return 0;
}
```

更多用例请查看[examples](examples)
