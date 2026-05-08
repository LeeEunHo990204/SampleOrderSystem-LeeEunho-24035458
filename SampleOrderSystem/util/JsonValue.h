#pragma once
#include <string>
#include <vector>
#include <map>
#include <variant>
#include <stdexcept>
#include <cstddef>

class JsonValue {
public:
    using Array  = std::vector<JsonValue>;
    using Object = std::map<std::string, JsonValue>;
    enum class Type { Null, Bool, Number, String, Array, Object };

    JsonValue()              : type_(Type::Null),   data_(std::monostate{}) {}
    JsonValue(bool v)        : type_(Type::Bool),   data_(v) {}
    JsonValue(double v)      : type_(Type::Number), data_(v) {}
    JsonValue(int v)         : type_(Type::Number), data_(static_cast<double>(v)) {}
    JsonValue(long long v)   : type_(Type::Number), data_(static_cast<double>(v)) {}
    JsonValue(const char* v) : type_(Type::String), data_(std::string(v)) {}
    JsonValue(std::string v) : type_(Type::String), data_(std::move(v)) {}
    JsonValue(Array v)       : type_(Type::Array),  data_(std::move(v)) {}
    JsonValue(Object v)      : type_(Type::Object), data_(std::move(v)) {}

    Type getType()  const noexcept { return type_; }
    bool isNull()   const noexcept { return type_ == Type::Null; }
    bool isBool()   const noexcept { return type_ == Type::Bool; }
    bool isNumber() const noexcept { return type_ == Type::Number; }
    bool isString() const noexcept { return type_ == Type::String; }
    bool isArray()  const noexcept { return type_ == Type::Array; }
    bool isObject() const noexcept { return type_ == Type::Object; }

    bool               getBool()   const { require(Type::Bool,   "bool");   return std::get<bool>(data_); }
    double             getNumber() const { require(Type::Number, "number"); return std::get<double>(data_); }
    const std::string& getString() const { require(Type::String, "string"); return std::get<std::string>(data_); }

    Array&       getArray()       { require(Type::Array,  "array");  return std::get<Array>(data_); }
    const Array& getArray()  const { require(Type::Array,  "array");  return std::get<Array>(data_); }
    Object&      getObject()      { require(Type::Object, "object"); return std::get<Object>(data_); }
    const Object& getObject() const { require(Type::Object, "object"); return std::get<Object>(data_); }

    JsonValue& operator[](const std::string& key) {
        require(Type::Object, "object");
        return std::get<Object>(data_)[key];
    }
    const JsonValue& operator[](const std::string& key) const {
        require(Type::Object, "object");
        return std::get<Object>(data_).at(key);
    }

    JsonValue& operator[](std::size_t idx) {
        require(Type::Array, "array");
        return std::get<Array>(data_)[idx];
    }
    const JsonValue& operator[](std::size_t idx) const {
        require(Type::Array, "array");
        return std::get<Array>(data_)[idx];
    }

    bool contains(const std::string& key) const noexcept {
        if (!isObject()) return false;
        return std::get<Object>(data_).count(key) > 0;
    }

    bool erase(const std::string& key) {
        if (!isObject()) return false;
        return std::get<Object>(data_).erase(key) > 0;
    }

    std::size_t size() const noexcept {
        if (isArray())  return std::get<Array>(data_).size();
        if (isObject()) return std::get<Object>(data_).size();
        return 0;
    }

    void push(JsonValue v) {
        require(Type::Array, "array");
        std::get<Array>(data_).push_back(std::move(v));
    }

private:
    void require(Type t, const char* name) const {
        if (type_ != t)
            throw std::runtime_error(std::string("JsonValue: expected ") + name);
    }

    Type type_;
    std::variant<std::monostate, bool, double, std::string, Array, Object> data_;
};
