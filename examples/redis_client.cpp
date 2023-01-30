#include <cassert>
#include <iostream>

#include "coro/coro.hpp"

using coro::redis::protocol::ArrayField;
using coro::redis::protocol::BulkStringField;
using coro::redis::protocol::FieldType;
using coro::redis::protocol::readField;
using coro::redis::protocol::SimpleStringField;
using coro::redis::protocol::writeField;
using coro::tcp::Conn;
using coro::tcp::connect;
using std::cout;
using std::dynamic_pointer_cast;
using std::endl;
using std::string;

void set(Conn conn, const string& key, const string& value) {
  auto req = ArrayField::null();
  req->mut_fields().push_back(BulkStringField::from("SET"));
  req->mut_fields().push_back(BulkStringField::from(key));
  req->mut_fields().push_back(BulkStringField::from(value));
  writeField(conn, req).await();

  auto resp = readField(conn).await();
  assert(resp->type() == FieldType::kSimpleString);
  auto str = dynamic_pointer_cast<SimpleStringField>(resp);
  cout << str->value() << endl;
}

void get(Conn conn, const string& key) {
  auto req = ArrayField::null();
  req->mut_fields().push_back(BulkStringField::from("GET"));
  req->mut_fields().push_back(BulkStringField::from(key));
  writeField(conn, req).await();

  auto resp = readField(conn).await();
  assert(resp->type() == FieldType::kBulkString);
  auto str = dynamic_pointer_cast<BulkStringField>(resp);
  cout << str->value() << endl;
}

int main() {
  auto conn = connect("127.0.0.1", 6379).await();
  set(conn, "aaa", "bbb");
  get(conn, "aaa");
  conn->close();
  return 0;
}
