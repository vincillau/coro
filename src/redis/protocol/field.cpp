#include "coro/redis/protocol/field.hpp"

namespace coro {
namespace redis {
namespace protocol {

void SimpleStringField::append(std::string& buf) const {
  buf.push_back('+');
  buf.append(value_);
  buf.append("\r\n");
}

void ErrorField::append(std::string& buf) const {
  buf.push_back('-');
  buf.append(value_);
  buf.append("\r\n");
}

void IntegerField::append(std::string& buf) const {
  buf.push_back(':');
  buf.append(std::to_string(value_));
  buf.append("\r\n");
}

std::string& BulkStringField::mut_value() {
  if (!value_.has_value()) {
    value_ = "";
  }
  return value_.value();
};

size_t BulkStringField::bytes() const {
  if (is_null()) {
    return sizeof("$-1\r\n") - 1;
  }
  return 1 + 9 + 2 + value_->length() + 2;
}

void BulkStringField::append(std::string& buf) const {
  if (is_null()) {
    buf.append("$-1\r\n");
    return;
  }
  // Redis 字符串最大长度 512 M，所以长度部分的长度不超过 9。
  buf.push_back('$');
  buf.append(std::to_string(value_->length()));
  buf.append("\r\n");
  buf.append(value_.value());
  buf.append("\r\n");
}

ArrayField::Fields& ArrayField::mut_fields() {
  if (!fields_.has_value()) {
    fields_ = Fields{};
  }
  return fields_.value();
};

size_t ArrayField::bytes() const {
  if (is_null()) {
    return sizeof("*-1\r\n") - 1;
  }
  // array 元素数目最大为 2^23-1。
  // 1 字节类型，至多 10 字节数字，2 字节 \r\n。
  size_t result = 1 + 10 + 2;
  const Fields& fields = fields_.value();
  for (std::shared_ptr<Field> field : fields) {
    result += field->bytes();
  }
  return result;
}

void ArrayField::append(std::string& buf) const {
  if (is_null()) {
    buf.append("*-1\r\n");
    return;
  }
  const Fields& fields = fields_.value();
  buf.push_back('*');
  buf.append(std::to_string(fields.size()));
  buf.append("\r\n");
  for (std::shared_ptr<Field> field : fields) {
    field->append(buf);
  }
}

}  // namespace protocol
}  // namespace redis
}  // namespace coro
