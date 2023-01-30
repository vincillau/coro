#include <string>

#include "coro/coro.hpp"

using coro::spawn;
using coro::http::protocol::readReq;
using coro::http::protocol::Response;
using coro::http::protocol::writeResp;
using coro::tcp::Conn;
using coro::tcp::listen;
using std::string;
using std::to_string;

void handler(Conn conn) {
  auto req = readReq(conn).await();
  Response resp;
  resp.code = 200;
  resp.reason = "OK";
  resp.version = "HTTP/1.1";
  string body = "<h1>You are accessing " + req.url + " </h1>";
  resp.headers = {{"Content-Type", "text/html"},
                  {"Content-Length", to_string(body.length())}};
  writeResp(conn, resp).await();
  conn->write(body.data(), body.length());
  conn->close();
}

int main() {
  auto listener = listen(8080);
  for (;;) {
    auto conn = listener->accept().await();
    spawn([conn]() { handler(conn); });
  }

  return 0;
}