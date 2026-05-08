#include "JsonParser.h"
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <cmath>

// -- Public API ---------------------------------------------------------------

JsonValue JsonParser::parse(const std::string& text) {
    JsonParser p(text);
    p.skipWhitespace();
    JsonValue result = p.parseValue();
    p.skipWhitespace();
    if (!p.atEnd())
        throw std::runtime_error("Unexpected trailing characters in JSON");
    return result;
}

std::string JsonParser::serialize(const JsonValue& value, bool pretty, int indent) {
    std::string pad(indent * 2, ' ');
    std::string childPad((indent + 1) * 2, ' ');
    std::ostringstream oss;

    switch (value.getType()) {
    case JsonValue::Type::Null:
        oss << "null";
        break;

    case JsonValue::Type::Bool:
        oss << (value.getBool() ? "true" : "false");
        break;

    case JsonValue::Type::Number: {
        double n = value.getNumber();
        if (std::floor(n) == n && std::abs(n) < 1e15)
            oss << static_cast<long long>(n);
        else
            oss << std::setprecision(10) << n;
        break;
    }

    case JsonValue::Type::String: {
        oss << '"';
        for (unsigned char c : value.getString()) {
            switch (c) {
            case '"':  oss << "\\\""; break;
            case '\\': oss << "\\\\"; break;
            case '\n': oss << "\\n";  break;
            case '\r': oss << "\\r";  break;
            case '\t': oss << "\\t";  break;
            case '\b': oss << "\\b";  break;
            case '\f': oss << "\\f";  break;
            default:
                if (c < 0x20) {
                    oss << "\\u" << std::hex << std::setw(4) << std::setfill('0') << (int)c;
                    oss << std::dec;
                } else {
                    oss << c;
                }
            }
        }
        oss << '"';
        break;
    }

    case JsonValue::Type::Array: {
        const auto& arr = value.getArray();
        if (arr.empty()) { oss << "[]"; break; }
        oss << '[';
        for (std::size_t i = 0; i < arr.size(); ++i) {
            if (pretty) oss << '\n' << childPad;
            oss << serialize(arr[i], pretty, indent + 1);
            if (i + 1 < arr.size()) oss << ',';
        }
        if (pretty) oss << '\n' << pad;
        oss << ']';
        break;
    }

    case JsonValue::Type::Object: {
        const auto& obj = value.getObject();
        if (obj.empty()) { oss << "{}"; break; }
        oss << '{';
        std::size_t i = 0;
        for (const auto& [key, val] : obj) {
            if (pretty) oss << '\n' << childPad;
            oss << '"' << key << '"' << (pretty ? ": " : ":");
            oss << serialize(val, pretty, indent + 1);
            if (i + 1 < obj.size()) oss << ',';
            ++i;
        }
        if (pretty) oss << '\n' << pad;
        oss << '}';
        break;
    }
    }
    return oss.str();
}

// -- Private helpers ----------------------------------------------------------

void JsonParser::skipWhitespace() {
    while (pos_ < text_.size()) {
        char c = text_[pos_];
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r') ++pos_;
        else break;
    }
}

char JsonParser::peek() const {
    return (pos_ < text_.size()) ? text_[pos_] : '\0';
}

char JsonParser::consume() {
    if (pos_ >= text_.size())
        throw std::runtime_error("Unexpected end of JSON input");
    return text_[pos_++];
}

void JsonParser::expect(char c) {
    char got = consume();
    if (got != c)
        throw std::runtime_error(
            std::string("Expected '") + c + "' but got '" + got + "'");
}

bool JsonParser::atEnd() const {
    return pos_ >= text_.size();
}

// -- Recursive descent parsers ------------------------------------------------

JsonValue JsonParser::parseValue() {
    skipWhitespace();
    char c = peek();
    if (c == '"') return parseString();
    if (c == '{') return parseObject();
    if (c == '[') return parseArray();
    if (c == 't') { pos_ += 4; return JsonValue(true); }
    if (c == 'f') { pos_ += 5; return JsonValue(false); }
    if (c == 'n') { pos_ += 4; return JsonValue(); }
    if (c == '-' || (c >= '0' && c <= '9')) return parseNumber();
    throw std::runtime_error(std::string("Unexpected JSON character: '") + c + "'");
}

std::string JsonParser::parseRawString() {
    expect('"');
    std::string result;
    while (true) {
        char c = consume();
        if (c == '"') break;
        if (c != '\\') { result += c; continue; }
        char esc = consume();
        switch (esc) {
        case '"':  result += '"';  break;
        case '\\': result += '\\'; break;
        case '/':  result += '/';  break;
        case 'n':  result += '\n'; break;
        case 'r':  result += '\r'; break;
        case 't':  result += '\t'; break;
        case 'b':  result += '\b'; break;
        case 'f':  result += '\f'; break;
        default:   result += esc;
        }
    }
    return result;
}

JsonValue JsonParser::parseString() {
    return JsonValue(parseRawString());
}

JsonValue JsonParser::parseNumber() {
    std::size_t start = pos_;
    if (peek() == '-') ++pos_;
    while (pos_ < text_.size() && text_[pos_] >= '0' && text_[pos_] <= '9') ++pos_;
    if (pos_ < text_.size() && text_[pos_] == '.') {
        ++pos_;
        while (pos_ < text_.size() && text_[pos_] >= '0' && text_[pos_] <= '9') ++pos_;
    }
    if (pos_ < text_.size() && (text_[pos_] == 'e' || text_[pos_] == 'E')) {
        ++pos_;
        if (pos_ < text_.size() && (text_[pos_] == '+' || text_[pos_] == '-')) ++pos_;
        while (pos_ < text_.size() && text_[pos_] >= '0' && text_[pos_] <= '9') ++pos_;
    }
    return JsonValue(std::stod(text_.substr(start, pos_ - start)));
}

JsonValue JsonParser::parseArray() {
    expect('[');
    skipWhitespace();
    JsonValue::Array arr;
    if (peek() == ']') { consume(); return JsonValue(arr); }
    while (true) {
        skipWhitespace();
        arr.push_back(parseValue());
        skipWhitespace();
        if (peek() == ']') { consume(); break; }
        expect(',');
    }
    return JsonValue(arr);
}

JsonValue JsonParser::parseObject() {
    expect('{');
    skipWhitespace();
    JsonValue::Object obj;
    if (peek() == '}') { consume(); return JsonValue(obj); }
    while (true) {
        skipWhitespace();
        std::string key = parseRawString();
        skipWhitespace();
        expect(':');
        skipWhitespace();
        obj[key] = parseValue();
        skipWhitespace();
        if (peek() == '}') { consume(); break; }
        expect(',');
    }
    return JsonValue(obj);
}
