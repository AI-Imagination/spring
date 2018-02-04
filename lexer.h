//
// Created by Yan Chunwei on 1/25/18.
//

#ifndef SPRING_LEXER_H
#define SPRING_LEXER_H

#include <array>
#include <cstdint>
#include <glog/logging.h>
#include <regex>
#include <string>
#include <utility>
#include <vector>

namespace spring {

// some helper macro to simplify operation on Token
#define _T(type__) ::spring::Token::Type::type__
#define _T_NAME(type__) ::spring::Token::typenames[_T(type__)]

/*
 * Token is the basic unit of parser.
 */
struct Token {
  enum class Type {
    SPACE = 0,
    NAME,    // _ab, ab23
    STRING,  // "xxx"
    FLOAT,   // 1.23
    INT,     // 123
    DOT,     // .
    EQ,      // =
    GT,      // >
    GE,      // >=
    LT,      // <
    LE,      // <=
    ADD,     // +
    MINUS,   // -
    MUL,     // *
    DIV,     // /
    LP,      // (
    RP,      // )
    COMMENT, // #...
    COMMA,   // ,
    ERROR,
    EOB
  };

  static constexpr int kNumTypes = int(Type::COMMA) - int(Type::SPACE) + 1;

  Type type;
  std::string text;
  uint32_t pos;
  uint32_t lineno;

  explicit Token(Type type) : type(type) {}
  Token(Type type, char c) : type(type), text(std::to_string(c)) {}
  Token(Type type, std::string &&text) : type(type), text(std::move(text)) {}
  Token(Type type, std::string &&text, size_t pos)
      : type(type), text(std::move(text)), pos(pos) {}

  const std::string type_name() const;
  std::string tostring() const;

  bool is_eob() const { return type == _T(EOB); }
  bool is_error() const { return type == _T(ERROR); }

  static const std::regex &rule(Type type) {
    return rules()[static_cast<int>(type)];
  }

  static const std::array<std::regex, kNumTypes> &rules() {
    InitTypes();
    return rules_;
  }

private:
  static void InitTypes();

private:
  static std::array<std::string, kNumTypes> typenames_;
  static std::array<std::regex, kNumTypes> rules_;
  static std::array<short, kNumTypes> priorities_;
  static bool inited;
};

static const char kEOF = '\0';

class Stream {
public:
  explicit Stream(const std::string &buffer) : buffer_(buffer) {}

  static bool is_digit(char c) { return std::isdigit(c); }
  static bool is_dot(char c) { return c == '.'; }
  static bool is_alpha(char c) { return std::isalpha(c); }

  char peek() {
    if (cursor_ >= buffer_.size())
      return kEOF;
    return buffer_[cursor_ + 1];
  }

  char fetch() {
    char c = peek();
    cursor_++;
    return c;
  }

private:
  std::string buffer_;
  size_t cursor_{0};
};

class TokenStream {
public:
  explicit TokenStream(const std::string &buffer) : buffer_(buffer) {}

  Token NextToken();

  char PeekChar() {
    if (cursor_ >= buffer_.size())
      return kEOF;
    return buffer_[cursor_];
  }

  void IgnoreSpace();

private:
  std::string buffer_;
  size_t cursor_{0};
};

} // namespace spring

#endif // SPRING_LEXER_H
