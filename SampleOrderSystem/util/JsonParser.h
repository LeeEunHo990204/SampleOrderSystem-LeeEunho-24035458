#pragma once
#include "JsonValue.h"
#include <string>

class JsonParser {
public:
    // Parse JSON text into a JsonValue. Throws std::runtime_error on failure.
    static JsonValue parse(const std::string& text);

    // Serialize a JsonValue back to a JSON string.
    // pretty=true: human-readable indented output
    static std::string serialize(const JsonValue& value, bool pretty = true, int indent = 0);

private:
    explicit JsonParser(const std::string& text) : text_(text), pos_(0) {}

    JsonValue   parseValue();
    JsonValue   parseString();
    JsonValue   parseNumber();
    JsonValue   parseArray();
    JsonValue   parseObject();
    std::string parseRawString();

    void skipWhitespace();
    char peek()  const;
    char consume();
    void expect(char c);
    bool atEnd() const;

    const std::string& text_;
    std::size_t        pos_;
};
