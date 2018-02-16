#include "lexer.h"

namespace spring {

std::array<std::string, Token::kNumTypes> Token::typenames_;
std::array<std::regex, Token::kNumTypes> Token::rules_;
std::array<short, Token::kNumTypes> Token::priorities_;
bool Token::inited{false};

void Token::InitTypes() {
  if (inited) return;

#define REGEX(type__, rule__)                             \
  rules_[static_cast<int>(_T(type__))] =                  \
      std::regex(rule__, std::regex_constants::extended); \
  typenames_[static_cast<int>(_T(type__))] = #type__;

  REGEX(SPACE, "[ \t\r]+");
  REGEX(NAME, "[a-zA-Z_]+[a-zA-Z_0-9]*");
  REGEX(STRING, R"abc(".*")abc");
  REGEX(FLOAT, "[+-]?[0-9]+[.][0-9]+");
  REGEX(INT, "[0-9]+");

  REGEX(DOT, "[.]");

  REGEX(GE, ">=");
  REGEX(LE, "<=");
  REGEX(EQ, "[=][=]");
  REGEX(ASSIGN, "[=]");
  REGEX(GT, "[>]");
  REGEX(LT, "<");

  REGEX(ADD, "[+]");
  REGEX(MINUS, "-");
  REGEX(MUL, "[*]");
  REGEX(DIV, "[/]");

  REGEX(LP, "[(]");
  REGEX(RP, "[)]");
  REGEX(COMMENT, "#.*");
  REGEX(COMMA, ",");
  REGEX(IF, "if");
  REGEX(WHILE, "while");
  REGEX(FUNCTION, "function");
  REGEX(RETURN, "return");
#undef REGEX
  inited = true;
}

const std::string Token::type_name() const {
  InitTypes();
  return typenames_[static_cast<int>(type)];
}

std::string Token::tostring() const {
  return "<Token " + type_name() + ":" + text + " pos:" + std::to_string(pos) +
         ">";
}

Token TokenStream::NextToken() {
  IgnoreSpace();
  if (cursor_ >= buffer_.size()) return Token(_T(EOB), "");
  std::smatch match;
  // TODO improve the performance by memo
  for (int type = 1; type < Token::kNumTypes; type++) {
    auto tmp = buffer_.substr(cursor_);
    if (std::regex_search(tmp, match, Token::rule(Token::Type(type))) &&
        match.position(0) == 0) {
      Token t(Token::Type(type), match.str(), cursor_);
      cursor_ += match.str().size();
      return t;
    }
  }
  return Token(_T(ERROR), "parsing error: " + buffer_.substr(cursor_), cursor_);
}

void TokenStream::IgnoreSpace() {
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

}  // namespace spring
