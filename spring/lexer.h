#ifndef SPRING_LEXER_H
#define SPRING_LEXER_H

#include <glog/logging.h>
#include <array>
#include <cstdint>
#include <regex>
#include <string>
#include <utility>
#include <vector>

namespace spring {

// some helper macro to simplify operation on Token
#define _T(type__) ::spring::Token::Type::type__
#define _T_NAME(type__) ::spring::Token::typenames[_T(type__)]

/*
 * Token is the basic unit of parser, it helps to represent the code's
 * structure. One Token represents one or more underlying instructions.
 */
struct Token {
  enum class Type {
    SPACE = 0,
    NAME,    // _ab, ab23
    STRING,  // "xxx"
    FLOAT,   // 1.23
    INT,     // 123
    DOT,     // .

    GE,      // >=
    LE,      // <=
    EQ,      // ==
    ASSIGN,  // =
    GT,      // >
    LT,      // <

    ADD,    // +
    MINUS,  // -
    MUL,    // *
    DIV,    // /

    LP,     // (
    RP,     // )
    LB,     // {
    RB,     // }
    COMMA,  // ,

    COMMENT,  // #...

    IF,        // if
    WHILE,     // while
    FUNCTION,  // function
    RETURN,    // return

    AST,  // ast subtree

    ERROR,
    EOB
  };

  static constexpr int kNumTypes = int(Type::ERROR) - int(Type::SPACE) + 1;

  Type type;
  std::string text;
  uint32_t lineno;
  uint32_t pos;  // position in a line

  explicit Token(Type type) : type(type) {}
  Token(Type type, char c) : type(type), text(std::to_string(c)) {}
  Token(Type type, std::string &&text) : type(type), text(std::move(text)) {}
  Token(Type type, std::string &&text, size_t pos)
      : type(type), text(std::move(text)), pos(pos) {}

  const std::string type_name() const;
  std::string tostring() const;
  friend std::ostream &operator<<(std::ostream &os, const Token &other) {
    os << other.tostring();
    return os;
  }

  bool is_eob() const { return type == _T(EOB); }
  bool is_error() const { return type == _T(ERROR); }
  bool is_if() const { return type == _T(IF); }
  bool is_while() const { return type == _T(WHILE); }
  bool is_function() const { return type == _T(FUNCTION); }
  bool is_return() const { return type == _T(RETURN); }
  bool is_left_paren() const { return type == _T(LP); }
  bool is_right_paren() const { return type == _T(RP); }
  bool is_left_brace() const { return type == _T(LB); }
  bool is_right_brace() const { return type == _T(RB); }
  bool is_space() const { return type == _T(SPACE); }
  bool is_comma() const { return type == _T(COMMA); }

  static const std::regex &rule(Type type) {
    return rules()[static_cast<int>(type)];
  }
  char prior() const { return priors_[static_cast<int>(type)]; }

  void SetType(Type t) { type = t; }

  static const std::array<std::regex, kNumTypes> &rules() {
    InitTypes();
    return rules_;
  }

 private:
  static void InitTypes();

 private:
  static std::array<std::string, kNumTypes> typenames_;
  static std::array<std::regex, kNumTypes> rules_;
  static std::array<char, kNumTypes> priors_;  // operator priorities.
  static bool inited;
};

static const char kEOF = '\0';
static const char kEOL = '\n';

class Stream {
 public:
  explicit Stream(const std::string &buffer) : buffer_(buffer) {}

  static bool is_digit(char c) { return std::isdigit(c); }
  static bool is_dot(char c) { return c == '.'; }
  static bool is_alpha(char c) { return std::isalpha(c); }

  char peek() {
    if (cursor_ >= buffer_.size()) return kEOF;
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

  Token NextToken() const;

  std::vector<Token> GetTokens() const;

  char PeekChar() {
    if (cursor_ >= buffer_.size()) return kEOF;
    return buffer_[cursor_];
  }

  void IgnoreSpace() const;

  std::string DebugString() const;

 private:
  std::string buffer_;
  mutable size_t cursor_{0};
};

}  // namespace spring

#endif  // SPRING_LEXER_H
