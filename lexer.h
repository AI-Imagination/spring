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

const int kNumTypes = 9;

// some helper macro to simplify operation on Token
#define _T(type__) ::spring::Token::Type::type__
#define _T_NAME(type__) ::spring::Token::typenames[_T(type__)]

/*
 * Token is the basic unit of parser.
 */
struct Token {
  enum Type {
    SPACE = 0,
    STRING,
    NAME,
    FLOAT,
    INT,
    COMMENT,
    DOT,
    EQ,
    COMMA,
    ERROR,
    EOB
  };

  Type type;
  std::string text;

  explicit Token(Type type) : type(type) { InitTypes(); }
  Token(Type type, char c) : type(type), text(std::to_string(c)) {
    InitTypes();
  }
  Token(Type type, std::string &&text) : type(type), text(std::move(text)) {
    InitTypes();
  }

  const std::string type_name() const { return typenames[type]; }

  std::string tostring() const {
    return "<Token " + type_name() + ":" + text + ">";
  }

  static const std::regex &rule(Type type) {
    InitTypes();
    return rules[type];
  }

private:
  static void InitTypes() {
    if (inited)
      return;

#define REGEX(type__, rule__)                                                  \
  rules[_T(type__)] = std::regex(rule__);                                      \
  typenames[_T(type__)] = #type__;

    rules.resize(kNumTypes);
    typenames.resize(kNumTypes);

    REGEX(COMMA, ",");
    REGEX(DOT, "[.]");
    REGEX(NAME, "[a-zA-Z_]+[a-zA-Z_0-9]*");
    REGEX(INT, "[a-zA-Z_]+[a-zA-Z_0-9]*");
    REGEX(FLOAT, "[+-]?[0-9]+[.][0-9]+");
    REGEX(STRING, "\".*\"");
    REGEX(COMMENT, "#.*");
    REGEX(EQ, "=")
    REGEX(SPACE, "[ \t\r]+");
#undef REGEX
    inited = true;
  }

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

  Token NextToken() {
    IgnoreSpace();
    if (cursor_ >= buffer_.size())
      return Token(_T(EOB), "");
    std::smatch match;
    // TODO improve the performance by memo
    for (int type = 1; type < kNumTypes; type++) {
      auto tmp = buffer_.substr(cursor_);
      if (std::regex_search(tmp, match,
                            Token::rule(Token::Type(type))) &&
          match.position(0) == 0) {
        cursor_ += match.str().size();
        return Token(Token::Type(type), match.str());
      }
    }
    return Token(_T(ERROR), "");
  }

  char PeekChar() {
    if (cursor_ >= buffer_.size())
      return kEOF;
    return buffer_[cursor_];
  }

  void IgnoreSpace() {
    char c;
    while (cursor_ < buffer_.size()) {
      c = buffer_[cursor_];
      switch (c) {
      case ' ':
      case '\t':
      case '\r':
        cursor_++;
        break;
      default:
        return;
      }
    }
  }

private:
  std::string buffer_;
  size_t cursor_{0};
};

class Lexier {
public:
  explicit Lexier(const std::string &buffer) : stream_(buffer) {}

  Token NextToken() {
    do {
      switch (stream_.peek()) {
      case ' ':
      case '\t':
      case '\r':
        continue;
      case ',':
        return Token(Token::COMMA, stream_.fetch());
      case '.':
        return Token(Token::DOT, stream_.fetch());
      }
    } while (stream_.peek() != kEOF);
  }

  std::string NAME() {
    std::string buf;
    do {
      buf.push_back(stream_.fetch());
    } while (stream_.peek() != kEOF &&
             (std::isalpha(stream_.peek()) || std::isdigit(stream_.peek())));
    return buf;
  }

private:
  Stream stream_;
  size_t cursor_{0};
};

} // namespace spring

#endif // SPRING_LEXER_H
