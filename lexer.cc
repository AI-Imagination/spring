#include "lexer.h"

namespace spring {

std::array<std::string, Token::kNumTypes> Token::typenames_;
std::array<std::regex, Token::kNumTypes> Token::rules_;
std::array<char, Token::kNumTypes> Token::priors_;
bool Token::inited{false};

void Token::InitTypes() {
  if (inited) return;

// The operator precedence reference to http://en.cppreference.com/w/cpp/language/operator_precedence
const char kNoPrior = -1;  // no priority for non-operator.
#define REGEX(type__, rule__, priority)                   \
  rules_[static_cast<int>(_T(type__))] =                  \
      std::regex(rule__, std::regex_constants::extended); \
  typenames_[static_cast<int>(_T(type__))] = #type__;     \
  priors_[static_cast<int>(_T(type__))] = priority;

  REGEX(SPACE, "[ \t\r]+", kNoPrior);
  REGEX(NAME, "[a-zA-Z_]+[a-zA-Z_0-9]*", kNoPrior);
  REGEX(STRING, R"abc(".*")abc", kNoPrior);
  REGEX(FLOAT, "[+-]?[0-9]+[.][0-9]+", kNoPrior);
  REGEX(INT, "[0-9]+", kNoPrior);

  REGEX(DOT, "[.]", 2);

  REGEX(GE, ">=", 9);
  REGEX(LE, "<=", 9);
  REGEX(EQ, "[=][=]", 10);
  REGEX(ASSIGN, "[=]", 16);
  REGEX(GT, "[>]", 9);
  REGEX(LT, "<", 9);

  REGEX(ADD, "[+]", 6);
  REGEX(MINUS, "-", 6);
  REGEX(MUL, "[*]", 5);
  REGEX(DIV, "[/]", 5);

  REGEX(LP, "[(]", kNoPrior);
  REGEX(RP, "[)]", kNoPrior);
  REGEX(COMMENT, "#.*", kNoPrior);
  REGEX(COMMA, ",", kNoPrior);
  REGEX(IF, "if", kNoPrior);
  REGEX(WHILE, "while", kNoPrior);
  REGEX(FUNCTION, "function", kNoPrior);
  REGEX(RETURN, "return", kNoPrior);
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
