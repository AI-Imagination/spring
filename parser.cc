#include "parser.h"
#include "lexer.h"

namespace spring {

void Parser::Parse(const std::vector<std::string> &codes,
                   std::vector<Token> *tokens) {
  for (uint32_t lineno = 0; lineno < codes.size(); lineno++) {
    const auto &line = codes[lineno];
    TokenStream ss(line);
    auto token = ss.NextToken();
    while(!token.is_eob()) {
      CHECK(!token.is_error()) << token.tostring();
      token.lineno = lineno;
      tokens->push_back(token);
      token = ss.NextToken();
    } while (!token.is_eob());
  }
}

/*
 * Parse a normal expression.
 * There are several patterns:
 * 1 + 2 ->             (+)
 *                      / \
 *                     1   2
 *
 * 1 - 2 * 3 ->         (-)
 *                    /     \
 *                   1      (*)
 *                         /   \
 *                        2     3
 *
 * 1 - 2 + 3
 *                       (+)
 *                     /     \
 *                   (-)      3
 *                  /   \
 *                 1     2
 */
void ParseExpression(const std::vector<Token> &tokens, uint32_t *cursor,
                     ast::AST *tree) {
  // get a expression
  for (; *cursor < tokens.size(); *cursor++) {
    const auto &token = tokens[*cursor];
    if (token.is_eob())
      break;
  }
}

} // namespace spring
