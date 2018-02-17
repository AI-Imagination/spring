#ifndef SPRING_UTILS_H
#define SPRING_UTILS_H

#include <glog/logging.h>

#include "memory.h"

namespace spring {

template <typename T>
struct List {
  struct Node {
    Node(const T& data) : data(data) {}

    T data;
    SharedPtr<Node> next;
  };

  using node_ptr = SharedPtr<Node>;

  void Append(const T& data) {
    auto n = MakeShared<Node>(data);
    if (!head) {
      head = n;
      tail = n;
    } else {
      CHECK(tail);
      tail->next = n;
      tail = n;
    }
  }

  void InsertAfter(node_ptr pre, node_ptr n) {
    n->next = pre->next;
    pre->next = n;
  }

  void RemoveAfter(node_ptr pre) {
    if ((!pre) || (!pre->next)) return;
    pre->next = pre->next->next;
  }

  // Remove the nodes between (begin_pre+1, end) inclusively.
  void RemoveAfter(node_ptr begin_pre, node_ptr end) {
    begin_pre->next = end->next;
  }

  void RemoveIf(std::function<bool(const T&)>& tester) {
    if (!head) return;
    auto p = head;
    while (p->next) {
      if (tester(p->next->data)) {
        p->next = p->next->next;
      } else
        p = p->next;
    }
    if (tester(head->data)) {
      head = head->next;
    }
  }

  node_ptr head;  // not included.
  node_ptr tail;
};

}  // namespace spring

#endif  // SPRING_UTILS_H
