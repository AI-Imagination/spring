//
// Created by Yan Chunwei on 1/25/18.
//

#ifndef SPRING_LEXER_H
#define SPRING_LEXER_H

#include <regex>
#include <string>
#include <vector>

namespace spring {
/*
 * Token is the basic unit of parser.
 */
struct Token {
  enum Type { SPACE = 0, COMMA, DOT, NAME, INT, FLOAT, STRING, COMMENT };

  Type type;
  std::string text;

  explicit Token(Type type) : type(type) {}
  Token(Type type, char c) : type(type), text(std::to_string(c)) {}
  Token(Type type, std::string &&text) : type(type), text(std::move(text)) {}

  std::string tostring() const {
    return "<Token " + std::to_string(type) + ":" + text + ">";
  }
};

#define _T(type__) ::spring::Token::Type::type__

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

  static const std::vector<std::regex>& rules() {
    static std::vector<std::regex> rules(8);
// TODO call once
#define REGEX(type__, rule__) rules[Token::Type::type__] = std::regex(rule__);
    REGEX(COMMA, ",");
    REGEX(DOT, ".");
    REGEX(NAME, "^[a-zA-Z_]+[a-zA-Z_0-9]*$");
    REGEX(INT, "^[a-zA-Z_]+[a-zA-Z_0-9]*$");
    REGEX(FLOAT, "^[+-]?[0-9]+[.][0-9]+$");
    REGEX(STRING, "^\".*\"$");
    REGEX(COMMENT, "^#.*$");
    REGEX(SPACE, "\s*");
#undef REGEX
    return rules;
  }

  Token NextToken() {
    IgnoreSpace();
    for (auto t :
         std::vector<Token::Type>({Token::Type::COMMENT, Token::Type::DOT})) {
    }
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
        break;
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
