#include "spring/parser.h"
#include <set>
#include "spring/utils.h"

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
Status ParseExpr(node_list* list, ast::node_ptr* ast) {
  // eat all parentheses
  EatParens(list);
  // keep merging the sub-expression by operator priority.
}

Status EatParens(node_list* list) {
  const int LP = -1;
  // search for left parentheses
  auto* p = list->head.get();
  std::vector<int> stack;
  while (p) {
    if (p->data->token->is_left_paren()) {
      node_list::Node* lp = p;
      node_list::Node* rp = p;
      // find right parentheses
      stack.clear();
      while (p) {
        if (p->data->token->is_left_paren())
          stack.push_back(LP);
        else if (p->data->token->is_right_paren()) {
          CHECK(!stack.empty());
          CHECK_EQ(stack.back(), LP);
          stack.pop_back();
        }
        if (stack.empty()) break;
        rp = p;
        p = p->next.get();
      }  // while
      // lp point to (
      // rp point node previous )
      rp->next = rp->next->next;  // skip )
      // remove from the original expression
      list->Remove(lp->next.get(), rp);
      rp->next = nullptr;
      node_list sublist(lp, rp);
      ast::node_ptr subtree;
      SP_CHECK_OK(ParseExpr(&sublist, &subtree));
      lp->data = subtree;
    }
    p = p->next.get();
  }
  return Status();
}

Status EatPriors(node_list* list) {
  std::set<char> priors;
  Visit<ast::node_ptr>(*list->head, *list->tail,
                       [&](const SharedPtr<ast::Node>& node) {
                         if (node->token->prior() != -1) {
                           priors.insert(node->token->prior());
                         }
                       });

  auto priors_sorted = std::vector<char>(priors.begin(), priors.end());
  sort(priors_sorted.begin(), priors_sorted.end());

  node_list::node_ptr left_arg;
  for (char prior : priors_sorted) {
    auto* p = list->head.get();
    while (p) {
      if (p->data->token->prior() == prior) {
        // remove the triple from the original list
        CHECK(p->next);
        list->Remove(left_arg, p->next);
      }
    }
  }
}

Status EatBinaryOp(node_list* list, node_list::Node* larg) {
  CHECK(larg->next);
  CHECK(larg->next->next);
  // remove the tripple
  list->Remove(larg->next, larg->next->next);

  auto ast = larg->next->data;
  ast->left = larg->data;
  ast->right = larg->next->next->data;
  larg->data = ast;
  return Status();
}

node_ptr Parser::EatExpr() {
  // keep scanning the highest priority operators.
  return spring::node_ptr();
}

}  // namespace spring