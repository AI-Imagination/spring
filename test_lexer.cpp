#include "lexer.h"

#include <gtest/gtest.h>

using namespace std;
using namespace spring;

TEST(lexer, tests) {
  auto t = _T(DOT);
  ASSERT_TRUE(regex_match(".", TokenStream::rules()[t]));
}

TEST(lexer, NAME) {
  ASSERT_TRUE(regex_match("_12ag", TokenStream::rules()[_T(NAME)]));
  ASSERT_FALSE(regex_match("12ag", TokenStream::rules()[_T(NAME)]));
}

TEST(lexer, FLOAT) {
  ASSERT_TRUE(regex_match("0.12", TokenStream::rules()[_T(FLOAT)]));
  ASSERT_TRUE(regex_match("+0.12", TokenStream::rules()[_T(FLOAT)]));
  ASSERT_TRUE(regex_match("-0.12", TokenStream::rules()[_T(FLOAT)]));
  ASSERT_TRUE(regex_match("-0.1", TokenStream::rules()[_T(FLOAT)]));
  ASSERT_FALSE(regex_match("-134", TokenStream::rules()[_T(FLOAT)]));
  ASSERT_FALSE(regex_match("134", TokenStream::rules()[_T(FLOAT)]));
}

TEST(lexer, STRING) {
  ASSERT_TRUE(regex_match("\"hello\"", TokenStream::rules()[_T(STRING)]));
  ASSERT_FALSE(regex_match("\"hello", TokenStream::rules()[_T(STRING)]));
}

TEST(lexer, COMMENT) {
  ASSERT_TRUE(regex_match("#", TokenStream::rules()[_T(COMMENT)]));
  ASSERT_TRUE(regex_match("# hello worldsdfa1324eqew", TokenStream::rules()[_T(COMMENT)]));
  ASSERT_FALSE(regex_match("hel # xxxxx", TokenStream::rules()[_T(COMMENT)]));
}
