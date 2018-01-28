//
// Created by Yan Chunwei on 1/25/18.
//

#ifndef SPRING_LEXER_H
#define SPRING_LEXER_H

#include <glog/logging.h>
#include <regex>
#include <string>
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
    NAME, // _ab, ab23
    // value
    STRING, // "xxx"
    FLOAT,  // 1.23
    INT,    // 123
    // single operator
    DOT, // .
    // binary operator
    EQ,    // =
    GT,    // >
    GE,    // >=
    LT,    // <
    LE,    // <=
    ADD,   // +
    MINUS, // -
    MUL,   // *
    DIV,   // /
    // syntax sugger
    LP,      // left parenthese
    RP,      // right parenthese
    COMMENT, // #...
    COMMA,   // ,
    ERROR,
    EOB
  };

  static constexpr int kNumTypes = int(Type::COMMA) - int(Type::SPACE) + 1;

  Type type;
  std::string text;

  explicit Token(Type type) : type(type) { InitTypes(); }
  Token(Type type, char c) : type(type), text(std::to_string(c)) {
    InitTypes();
  }
  Token(Type type, std::string &&text) : type(type), text(std::move(text)) {
    InitTypes();
  }

  const std::string type_name() const {
    return typenames[static_cast<int>(type)];
  }

  std::string tostring() const {
    return "<Token " + type_name() + ":" + text + ">";
  }

  static const std::regex &rule(Type type) {
    InitTypes();
    return rules[static_cast<int>(type)];
  }

private:
  static void InitTypes();

private:
  static std::vector<std::string> typenames;
  static std::vector<std::regex> rules;
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
