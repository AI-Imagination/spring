#include "lexer.h"

#include <glog/logging.h>
#include <gtest/gtest.h>

using namespace std;
using namespace spring;

TEST(lexer, tests) {
  ASSERT_TRUE(regex_match(".", Token::rule(_T(DOT))));
  ASSERT_FALSE(regex_match("+", Token::rule(_T(DOT))));
}

TEST(lexer, NAME) {
  ASSERT_TRUE(regex_match("_12ag", Token::rule(_T(NAME))));
  ASSERT_FALSE(regex_match("12ag", Token::rule(_T(NAME))));
}

TEST(lexer, FLOAT) {
  ASSERT_TRUE(regex_match("0.12", Token::rule(_T(FLOAT))));
  ASSERT_TRUE(regex_match("+0.12", Token::rule(_T(FLOAT))));
  ASSERT_TRUE(regex_match("-0.12", Token::rule(_T(FLOAT))));
  ASSERT_TRUE(regex_match("-0.1", Token::rule(_T(FLOAT))));
  ASSERT_FALSE(regex_match("-134", Token::rule(_T(FLOAT))));
  ASSERT_FALSE(regex_match("134", Token::rule(_T(FLOAT))));
}

TEST(lexer, STRING) {
  ASSERT_TRUE(regex_match("\"hello\"", Token::rule(_T(STRING))));
  ASSERT_FALSE(regex_match("\"hello", Token::rule(_T(STRING))));
}

TEST(lexer, COMMENT) {
  ASSERT_TRUE(regex_match("#", Token::rule(_T(COMMENT))));
  ASSERT_TRUE(
      regex_match("# hello worldsdfa1324eqew", Token::rule(_T(COMMENT))));
  ASSERT_FALSE(regex_match("hel # xxxxx", Token::rule(_T(COMMENT))));
}

TEST(lexer, SPACE) {
  ASSERT_TRUE(regex_match("  ", Token::rule(_T(SPACE))));
  ASSERT_TRUE(regex_match(" \t ", Token::rule(_T(SPACE))));
  ASSERT_TRUE(regex_match(" \r ", Token::rule(_T(SPACE))));
  ASSERT_FALSE(regex_match(" \r ,", Token::rule(_T(SPACE))));
}

TEST(lexer, NextToken) {
  string buffer = "a = 12.34";
  TokenStream ss(buffer);
  auto token = ss.NextToken();
  vector<Token::Type> types({_T(NAME), _T(EQ), _T(FLOAT)});
  int offset = 0;
  while (token.type != _T(EOB) && token.type != _T(ERROR)) {
    LOG(INFO) << token.tostring();
    EXPECT_EQ(types[offset++], token.type);
    token = ss.NextToken();
  }
}

TEST(lexer, NextToken1) {
  string buffer = "a = _b12_221_a + (b < 13)";
  TokenStream ss(buffer);
  vector<Token::Type> types({_T(NAME), _T(EQ), _T(NAME), _T(ADD), _T(LP),
                             _T(NAME), _T(LT), _T(INT), _T(RP)});
  auto token = ss.NextToken();

  int offset = 0;
  while (token.type != _T(EOB)) {
    LOG(INFO) << token.tostring();
    ASSERT_TRUE(token.type != _T(ERROR));
    EXPECT_EQ(token.type, types[offset++]);
    token = ss.NextToken();
  }
}
