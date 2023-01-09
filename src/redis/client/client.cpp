#include "coro/redis/client/client.hpp"

namespace coro {
namespace redis {
namespace client {

void Client::close() {
  if (conn_) {
    conn_->close();
    conn_.reset();
  }
}

Promise<void> Client::connect(const std::string& host, uint16_t port) {
  Promise<void> promise;
  tcp::connect(host, port)
      .then([this, promise](tcp::Conn conn) {
        conn_ = conn;
        promise.resolve();
      })
      .except([promise](tcp::Conn conn, std::error_code error) {
        promise.reject(std::move(error));
      });
  return promise;
}

Promise<Client::Response> Client::exec(Request req) {
  Promise<std::shared_ptr<protocol::Field>> promise;
  auto array = protocol::ArrayField::null();
  for (std::string& s : req) {
    auto field = protocol::BulkStringField::from(std::move(s));
    array->mut_fields().push_back(field);
  }
  auto conn = conn_;
  protocol::writeField(conn_, array)
      .then([this, promise]() {
        protocol::readField(conn_)
            .then([promise](std::shared_ptr<protocol::Field> resp) {
              promise.resolve(resp);
            })
            .except([promise](std::shared_ptr<protocol::Field> resp,
                              std::error_code error) {
              promise.reject(std::move(error));
            });
      })
      .except([promise](std::error_code error) {
        promise.reject(std::move(error));
      });
  return promise;
}

}  // namespace client
}  // namespace redis
}  // namespace coro
