#ifndef SPRING_PARSER_H
#define SPRING_PARSER_H

#include <cstdint>
#include <functional>
#include <glog/logging.h>
#include "lexer.h"

namespace spring {
namespace ast {

// node index type.
using nidx_t = uint32_t;

/*
 * Node of a AST. A expression will be translated to a AST, so that number of
 * nodes is limited, and can be stored in a vector.
 */
struct Node {
  Token token;
  std::vector<nidx_t> children;
};

struct AST {
  using visitor_t = std::function<void(const Token &token)>;
  std::vector<Node> nodes;

  size_t size() const { return nodes.size(); }
  const Node &root() const { return nodes.front(); }
  nidx_t AddChildren(nidx_t p, Node &&node);
  void PostOrderTranspose(visitor_t &visitor);
};

} // namespace ast

/*
 * Input lines of codes, a Parser will translated each expression to an AST, and
 * call instruction-generator to translate to instructions.
 */
class Parser {
public:
  void Parse(const std::vector<std::string> &codes, std::vector<Token> *tokens);
  void BuildAST(const std::vector<Token> &tokens, ast::AST *ast);
  void AST2Codes(const ast::AST &ast);

private:
};

} // namespace spring
#endif