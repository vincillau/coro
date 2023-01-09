#ifndef CORO_INCLUDE_CORO_REDIS_PROTOCOL_FIELD_HPP_
#define CORO_INCLUDE_CORO_REDIS_PROTOCOL_FIELD_HPP_

#include <boost/optional.hpp>
#include <memory>
#include <string>
#include <vector>

namespace coro {
namespace redis {
namespace protocol {

enum class FieldType {
  kSimpleString,
  kError,
  kInteger,
  kBulkString,
  kArray,
};

class Field {
 public:
  virtual FieldType type() const = 0;
  /**
   * @brief 字段序列化后的大小（估算）。
   * @return size_t 字段序列化后的大小
   */
  virtual size_t bytes() const = 0;
  virtual void append(std::string& buf) const = 0;
};

class SimpleStringField : public Field {
 public:
  SimpleStringField() = default;
  explicit SimpleStringField(std::string value) : value_(std::move(value)) {}

  FieldType type() const override { return FieldType::kSimpleString; }
  size_t bytes() const override { return 1 + value_.length() + 2; }

  const std::string& value() const { return value_; };
  std::string& mut_value() { return value_; };
  void set_value(std::string value) { value_ = std::move(value); };

  void append(std::string& buf) const override;

  static std::shared_ptr<SimpleStringField> from(std::string value) {
    return std::make_shared<SimpleStringField>(std::move(value));
  }

 private:
  std::string value_;
};

class ErrorField : public Field {
 public:
  ErrorField() = default;
  explicit ErrorField(std::string value) : value_(std::move(value)) {}

  FieldType type() const override { return FieldType::kError; }
  size_t bytes() const override { return 1 + value_.length() + 2; }

  const std::string& value() const { return value_; };
  std::string& mut_value() { return value_; };
  void set_value(std::string value) { value_ = std::move(value); };

  void append(std::string& buf) const override;

  static std::shared_ptr<ErrorField> from(std::string value) {
    return std::make_shared<ErrorField>(std::move(value));
  }

 private:
  std::string value_;
};

class IntegerField : public Field {
 public:
  IntegerField() = default;
  explicit IntegerField(int64_t value) : value_(value) {}

  FieldType type() const override { return FieldType::kInteger; }
  size_t bytes() const override {
    // 1 字节类型，至多 20 字节数字，2 字节 \r\n。
    return 1 + 20 + 2;
  }

  int64_t value() const { return value_; };
  int64_t& mut_value() { return value_; };
  void set_value(int64_t value) { value_ = value; };

  void append(std::string& buf) const override;

  static std::shared_ptr<IntegerField> from(int64_t value) {
    return std::make_shared<IntegerField>(std::move(value));
  }

 private:
  int64_t value_;
};

class BulkStringField : public Field {
 public:
  BulkStringField() = default;
  explicit BulkStringField(std::string value) : value_(std::move(value)) {}

  FieldType type() const override { return FieldType::kBulkString; }
  size_t bytes() const override;

  bool is_null() const { return !value_.has_value(); }
  void set_null() { value_ = {}; }
  const std::string& value() const { return value_.value(); };
  std::string& mut_value();
  void set_value(std::string value) { value_ = std::move(value); };

  void append(std::string& buf) const override;

  static std::shared_ptr<BulkStringField> null() {
    return std::make_shared<BulkStringField>();
  }
  static std::shared_ptr<BulkStringField> from(std::string value) {
    return std::make_shared<BulkStringField>(std::move(value));
  }

 private:
  boost::optional<std::string> value_;
};

class ArrayField : public Field {
 public:
  using Fields = std::vector<std::shared_ptr<Field>>;

  ArrayField() = default;

  FieldType type() const override { return FieldType::kArray; }
  size_t bytes() const override;

  bool is_null() const { return !fields_.has_value(); }
  void set_null() { fields_ = {}; }
  const Fields& fields() const { return fields_.value(); }
  Fields& mut_fields();

  void append(std::string& buf) const override;

  static std::shared_ptr<ArrayField> null() {
    return std::make_shared<ArrayField>();
  }

 private:
  boost::optional<Fields> fields_;
};

}  // namespace protocol
}  // namespace redis
}  // namespace coro

#endif  // CORO_INCLUDE_CORO_REDIS_PROTOCOL_FIELD_HPP_
