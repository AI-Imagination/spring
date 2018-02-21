#ifndef SPRING_PARSER_H
#define SPRING_PARSER_H

#include <cstdint>
#include <functional>
#include <memory>

#include "spring/lexer.h"
#include "spring/memory.h"
#include "spring/status.h"
#include "spring/utils.h"

namespace spring {
namespace ast {

// node index type.
using nidx_t = uint32_t;

/*
 * Node of a AST. A expression will be translated to a AST, so that number of
 * nodes is limited, and can be stored in a vector.
 */
struct Node {
  Node(const Token &token) : token(token) {}

  Token token;
  SharedPtr<Node> left;
  SharedPtr<Node> right;
};

using node_ptr = SharedPtr<Node>;

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

  void operator()();

  const Token &Peek(int offset = 0) const {
    return tokens_[token_offest_ + offset];
  }

 private:
  const std::vector<Token> &tokens_;
  // The offset of current token.
  uint32_t token_offest_{0};
};

using node_list = List<node_ptr>;

/*
 * Tell whether the preceding tokens is a function call.
 *
 * Such as
 * a = getName("candy")
 */
bool PrecedFunctionCall(const List<ast::node_ptr> &list);

/*
 * Tell whether the preceding tokens is a function definition.
 *
 * Such as
 *   function getName(name)
 */
bool PrecedFunctionDef(const List<ast::node_ptr> &list);

/*
 * Tell whether the preceding tokens is a if.
 *
 * Such as
 *   if (a > 10)
 */
bool PrecedIf(const List<ast::node_ptr> &list);

/*
 * Tell whether the preceding tokens is a while.
 *
 * Such as
 *    while (a > 10)
 */
bool PrecedWhile(const List<ast::node_ptr> &list);

/*
 * Tell whether the preceding tokens define an variable.
 *
 * Such as
 *    var a = 100
 */
bool PrecedVarDef(const List<ast::node_ptr> &list);

/*
 * Parse and clear the contents within the parentheses, generate ASTs and insert
 * back into the list.
 */
Status EatParens(node_list *list);

/*
 * Process the operators by the precedence order, the most important operators
 * first. Build an AST with operator and left and right arguments.
 */
Status EatPriors(node_list *list);

/*
 * 2 + 1 ->
 *
 *   (+)
 *   / \
 *  2   1
 */
Status EatBinaryOp(node_list *list, const node_list::Node *larg);

Status EatFunctionCalls(node_list *list);

Status EatFunctionCall(node_list *list, const node_list::Node *n);

Status EatFunctionDef(TokenStream &ts);

Status EatIf(TokenStream &ts);

Status EatWhile(TokenStream &ts);

/*
 * Create a list according to the input tokens.
 */
Status Tokens2List(const std::vector<Token> &tokens, List<ast::node_ptr> *list);

namespace debug {
std::string DisplayTokenNodeList(const node_list &node);
}  // namespace debug

}  // namespace spring
#endif
