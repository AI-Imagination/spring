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

Status EatPrior(node_list* list, char prior);
Status EatPriors(node_list* list) {
  VLOG(4) << "get list " << debug::DisplayTokenNodeList(*list);
  std::set<char> priors;
  auto* p = list->head.get();
  while (p) {
    const auto& token = p->data->token;
    if (token->prior() != -1) {
      CHECK(!token->is_left_paren());
      CHECK(!token->is_right_paren());
      priors.insert(token->prior());
    }
    p = p->next.get();
  }
  auto priors_sorted = std::vector<char>(priors.begin(), priors.end());
  sort(priors_sorted.begin(), priors_sorted.end());

  for (char prior : priors_sorted) {
    VLOG(4) << "processing prior " << int(prior);
    SP_CHECK_OK(EatPrior(list, prior));
    VLOG(4) << "list.size " << list->heavy_size();
  }
  return Status();
}

Status EatPrior(node_list* list, char prior) {
  CHECK(!list->Empty());
  debug::ValidList(*list);
  auto* p = list->head.get();
  while (p) {
    if (p->data->token->prior() == prior) {
      p = p->pre;
      SP_CHECK_OK(EatBinaryOp(list, p));
      VLOG(4) << "after EatBinaryOp " << debug::DisplayTokenNodeList(*list);
    }
    p = p->next.get();
  }
  return Status();
}

Status EatBinaryOp(node_list* list, const node_list::Node* larg_raw) {
  CHECK(larg_raw->next);
  CHECK(larg_raw->next->next);
  auto op = larg_raw->next;
  auto larg = list->head == larg_raw ? list->head : larg_raw->pre->next;
  auto rarg = op->next;
  VLOG(4) << "EatBinaryOp: " << larg->data->token->tostring() << " "
          << op->data->token->tostring() << " "
          << rarg->data->token->tostring();
  // remove the tripple
  list->Remove(larg->next, rarg);
  VLOG(4) << "EatBinaryOp: "
          << "after remove: " << debug::DisplayTokenNodeList(*list);
  op->next.Reset();
  rarg->next.Reset();
  // the center of this tripple should be an OP
  CHECK_NE(op->data->token->prior(), -1);
  const_cast<Token*>(op->data->token)->SetType(_T(AST));
  op->data->left = larg->data;
  op->data->right = rarg->data;
  larg->data = op->data;
  return Status();
}

node_ptr Parser::EatExpr() {
  // keep scanning the highest priority operators.
  return spring::node_ptr();
}

Status Tokens2List(const std::vector<Token>& tokens,
                   List<ast::node_ptr>* list) {
  // skip space
  for (const auto& token : tokens) {
    if (token.is_space()) continue;
    auto node = MakeShared<ast::Node>(&token);
    list->PushBack(node);
  }
  return Status();
}

std::string debug::DisplayTokenNodeList(const node_list& node) {
  std::stringstream ss;
  const auto* p = node.head.get();
  while (p) {
    ss << p->data->token->tostring();
    p = p->next.get();
  }
  return ss.str();
}

}  // namespace spring
