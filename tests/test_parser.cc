#include "spring/parser.h"

#include <gtest/gtest.h>

using namespace spring;

class ParserTester : public ::testing::Test {
 public:
  virtual void SetUp() override {
    const std::string buffer = "2*(3+2) / 2.3";

    // generate tokens
    TokenStream lexer(buffer);
    LOG(INFO) << lexer.DebugString();
    tokens = lexer.GetTokens();
    // generate list
    auto status = Tokens2List(tokens, &list);
    ASSERT_TRUE(status.ok());
  }

  node_list list;
  std::vector<Token> tokens;
};

TEST_F(ParserTester, Tokens2List) {
  ASSERT_EQ(tokens.size(), 9);
  ASSERT_EQ(list.heavy_size(), 9);
}
TEST_F(ParserTester, EatBinaryOp) {
  const auto &larg = list[3];
  auto p = list.head;
  auto pre = p;
  for (int i = 0; i < 3; i++) {
    pre = p;
    p = p->next;
  }
  LOG(INFO) << "memory: " << Memory::Global().size();
  LOG(INFO) << "stop at " << p->data->token.tostring();
  auto status = EatBinaryOp(&list, p.get());

  LOG(INFO) << "after eat binary op: " << debug::DisplayTokenNodeList(list);

  ASSERT_TRUE(status.ok());
  auto ast = pre->next->data;
  LOG(INFO) << "ast: " << ast->token.tostring();
  EXPECT_EQ(ast->token.type, _T(AST));
  EXPECT_EQ(ast->token.text, "+");
  EXPECT_EQ(ast->left->token.text, "3");
  EXPECT_EQ(ast->right->token.text, "2");
  debug::ValidList(list);
}
TEST(Parser, EatBinaryOp_head) {
  {
    const std::string buffer = "2*3+2 / 2.3";
    // generate tokens
    TokenStream lexer(buffer);
    LOG(INFO) << lexer.DebugString();
    auto tokens = lexer.GetTokens();
    node_list list;
    // generate list
    auto status = Tokens2List(tokens, &list);
    status = EatBinaryOp(&list, list.head.get());
    ASSERT_TRUE(status.ok());
    EXPECT_EQ(list.head->data->token.type, _T(AST));
    LOG(INFO) << "after EatBinaryOp " << debug::DisplayTokenNodeList(list);
  }
  ASSERT_EQ(Memory::Global().size(), 0);
}
TEST(Parser, EatPriors) {
  {
    const std::string buffer = "2*3+2 / 2.3";
    // generate tokens
    TokenStream lexer(buffer);
    LOG(INFO) << lexer.DebugString();
    auto tokens = lexer.GetTokens();
    node_list list;
    // generate list
    auto status = Tokens2List(tokens, &list);
    LOG(INFO) << "get list " << debug::DisplayTokenNodeList(list);
    ASSERT_TRUE(status.ok());

    status = EatPriors(&list);
    ASSERT_TRUE(status.ok());
  }
  ASSERT_EQ(Memory::Global().size(), 0);
}
