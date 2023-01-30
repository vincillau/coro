#include <iostream>
#include <vector>

#include "coro/coro.hpp"

using coro::http::protocol::readResp;
using coro::http::protocol::Request;
using coro::http::protocol::writeReq;
using coro::tcp::connect;
using std::cout;
using std::endl;
using std::pair;
using std::string;

// httpbin.org
const char* IP = "34.224.50.110";

string lowercase(const string& s) {
  string res;
  for (char c : s) {
    if (c >= 'A' && c <= 'Z') {
      res.push_back(c - 'A' + 'a');
    } else {
      res.push_back(c);
    }
  }
  return res;
}

int main() {
  auto req = Request();
  req.method = "GET";
  req.url = "/get";
  req.version = "HTTP/1.1";
  req.headers = {{"Host", "httpbin.org"},
                 {"Accept", "application/json"},
                 {"User-Agent", "coro"}};
  auto conn = connect(IP, 80).await();
  writeReq(conn, req).await();
  auto resp = readResp(conn).await();

  cout << resp.code << " " << resp.reason << " " << resp.version << endl;
  int content_length = 0;
  for (const pair<string, string>& header : resp.headers) {
    cout << header.first << ": " << header.second << endl;
    if (lowercase(header.first) == "content-length") {
      content_length = atoi(header.second.c_str());
    }
  }

  auto body = new char[content_length + 1];
  conn->readn(body, content_length).await();
  body[content_length] = '\0';
  cout << body << endl;
  delete[] body;

  conn->close();
  return 0;
}
