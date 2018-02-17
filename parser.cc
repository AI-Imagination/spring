#include "parser.h"
#include "utils.h"

namespace spring {

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

Status MatchParenPair(const std::vector<Token>& tokens, int offset,
                      std::pair<int, int>* res) {
  CHECK(tokens[offset].is_left_paren());
  const int kLP = -1;
  const int kRP = 1;
  std::vector<int> stack({kLP});
  res->first = offset;
  for (int i = offset + 1; i < tokens.size(); i++) {
    if (tokens[i].is_left_paren()) {
      stack.push_back(kLP);
    } else if (tokens[i].is_right_paren()) {
      CHECK(!tokens.empty());
      if (stack.back() == kLP) {
        stack.pop_back();
      }
    }
    if (stack.empty()) {
      res->second = i;
      break;
    }
  }
  return Status();
}

/*
 * Parse an expression, no () is allowed in an expression.
 *
 * 3 * ( 2 + 1 ) will be split to two expressions:
 *    - 3
 *    - 2 + 1
 *
 * Input a list of nodes, it parse a range of nodes and generate an AST and
 * insert back to the original list.
 */
using node_list = List<node_ptr>;
Status ParseExpr(node_list& list, const node_list::node_ptr& begin_pre,
                 const node_list::node_ptr& end) {
  // remove the range from list
  list.RemoveAfter(begin_pre, end);
  end->next = nullptr;

  // skip space
  auto p = begin_pre->next;
  while (p != end && p->data->token->is_space()) {
    p = p->next;
  }
  // get all the parentheses
  auto q = p;
  do {
  } while (q != end);

  // insert back to the original expression
}

node_ptr Parser::EatExpr() {
  int old_offset = token_offest_;
  // scan all the parentheses.
  int offset = token_offest_;
  std::pair<int, int> pares;
  while (offset < tokens_.size()) {
    if (tokens_[offset].is_space()) {
    } else if (tokens_[offset].is_comma())
      break;
    else if (tokens_[offset].is_left_paren()) {
      // search for the right brace
      std::pair<int, int> paren;
      SP_CHECK_OK(MatchParenPair(tokens_, offset, &paren));
      // remove the sub-expression from current expressoin.
      // EatExpr again
    }
    offset++;
  }

  // keep scanning the highest priority operators.
  return spring::node_ptr();
}

}  // namespace spring
