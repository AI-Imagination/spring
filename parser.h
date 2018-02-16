#ifndef SPRING_PARSER_H
#define SPRING_PARSER_H

#include <cstdint>
#include <functional>
#include <memory>

#include "lexer.h"
#include "memory.h"
#include "status.h"

namespace spring {
namespace ast {

// node index type.
using nidx_t = uint32_t;

/*
 * Node of a AST. A expression will be translated to a AST, so that number of
 * nodes is limited, and can be stored in a vector.
 */
struct Node {
  const Token *token;
  SharedPtr<Node> left;
  SharedPtr<Node> right;
};

struct AST {
  using visitor_t = std::function<void(const Token &token)>;
  Node *root;
  std::vector<Node> nodes;

  size_t size() const { return nodes.size(); }
  nidx_t AddChildren(nidx_t p, Node &&node);
  void PostOrderTranspose(visitor_t &visitor);
};

}  // namespace ast

using node_ptr = SharedPtr<ast::Node>;

struct Block {
  static std::vector<Block> &All() {
    static auto *_ = new std::vector<Block>;
    return *_;
  }
  static Block *New() {
    All().emplace_back();
    return &All().back();
  }
  static Block &at(size_t id) { return All().at(id); }

  Block *parent;
  // ASTs for statements.
  std::vector<node_ptr> stmts;
  std::vector<Token> tokens;
};

/*
 * Input lines of codes, a Parser will translated each expression to an AST, and
 * call instruction-generator to translate to instructions.
 */
class Parser {
 public:
  Parser(const std::vector<Token> &tokens) : tokens_(tokens) {}

  void operator()() {
    node_ptr node;
    while (token_offest_ < tokens_.size()) {
      if (Peek().is_function()) {
        node = EatFunctionDef();
      } else if (Peek().is_if()) {
        Block *new_block = Block::New();
        EatIf();
      } else if (Peek().is_while()) {
        EatExpr();
      } else {
        node = EatStmt();
      }
    }
  }

  const Token &Peek(int offset = 0) const {
    return tokens_[token_offest_ + offset];
  }

 protected:
  void ConsumeOneToken() { token_offest_++; }
  /*
   * Match an expression, for example:
   * b + 1
   * (b+1)*2
   * (b+1)
   */
  node_ptr EatExpr();
  /*
   * Match a statement, with ()s
   * (a+1)
   * a = b, c = d
   */
  node_ptr EatStmt();
  /*
   * Declare a variable, such as
   * var a
   * var a = 1 + 2
   */
  bool EatDecl();
  /*
   * A function declaration.
   *
   * function name() {
   *   var tmp = "hello"
   *   return tmp + " world"
   * }
   */
  node_ptr EatFunctionDef();
  /*
   * A while-loop
   *
   * while(a > 0) {
   *   a--
   * }
   */
  node_ptr EatWhile();
  /*
   * If statement.
   *
   * if (a > 0) {
   *   ...
   * } else {
   *   ...
   * }
   */
  node_ptr EatIf();

  bool EatLeftParen() const;
  bool EatRightParen() const;
  bool EatBinaryOp() const;
  bool EatLeftBrace() const;
  bool EatRightBrace() const;

 private:
  const std::vector<Token> &tokens_;
  // The offset of current token.
  uint32_t token_offest_{0};
};

}  // namespace spring
#endif
