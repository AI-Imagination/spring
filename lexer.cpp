#include "lexer.h"

namespace spring {

std::vector<std::string> Token::typenames_;
std::vector<std::regex> Token::rules_;
bool Token::inited{false};

void Token::InitTypes() {
  if (inited)
    return;

#define REGEX(type__, rule__)                                                  \
  rules_[static_cast<int>(_T(type__))] =                                       \
      std::regex(rule__, std::regex_constants::extended);                      \
  typenames_[static_cast<int>(_T(type__))] = #type__;

  rules_.resize(Token::kNumTypes);
  typenames_.resize(Token::kNumTypes);

  REGEX(SPACE, "[ \t\r]+");
  REGEX(NAME, "[a-zA-Z_]+[a-zA-Z_0-9]*");
  REGEX(STRING, R"abc(".*")abc");
  REGEX(FLOAT, "[+-]?[0-9]+[.][0-9]+");
  REGEX(INT, "[0-9]+");

  REGEX(DOT, "[.]");

  REGEX(EQ, "[=]");
  REGEX(GT, "[>]");
  REGEX(GE, ">=");
  REGEX(LT, "<");
  REGEX(LE, "<=");
  REGEX(ADD, "[+]");
  REGEX(MINUS, "-");
  REGEX(MUL, "[*]");
  REGEX(DIV, "[/]");

  REGEX(LP, "[(]");
  REGEX(RP, "[)]");
  REGEX(COMMENT, "#.*");
  REGEX(COMMA, ",");
#undef REGEX
  inited = true;
}

Token TokenStream::NextToken() {
  IgnoreSpace();
  if (cursor_ >= buffer_.size())
    return Token(_T(EOB), "");
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

} // namespace spring
