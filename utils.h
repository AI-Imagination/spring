#ifndef SPRING_UTILS_H
#define SPRING_UTILS_H

#include <glog/logging.h>
#include <functional>

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

  List() = default;

  List(const node_ptr& begin, const node_ptr& end) : head(begin), tail(end) {}

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

  void InsertAfter(const node_ptr& pre, const node_ptr& n) {
    n->next = pre->next;
    pre->next = n;
  }

  void PushFront(const T& data) {
    auto n = MakeShared<Node>(data);
    if (head) {
      n->next = head->next;
    }
    head = n;
  }

  void PopFront() {
    if (!head) return;
    head = head->next;
  }

  void RemoveAfter(node_ptr pre) {
    // TODO update tail
    if ((!pre) || (!pre->next)) return;
    pre->next = pre->next->next;
  }

  // Remove the nodes between (begin_pre+1, end) inclusively.
  template <typename Ptr>
  void RemoveAfter(Ptr begin_pre, Ptr end) {
    // TODO update tail
    begin_pre->next = end->next;
  }

  void RemoveIf(const std::function<bool(const T&)>& tester) {
    // TODO update tail
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

  size_t heavy_size() const {
    auto* p = head.get();
    size_t count = 0;
    while (p) {
      count++;
      p = p->next.get();
    }
    return count;
  }

  node_ptr head;  // not included.
  node_ptr tail;
};

// both begin and end are inclusive.
template <typename T>
void Visit(const typename List<T>::Node& begin,
           const typename List<T>::Node& end,
           const std::function<void(const T&)>& handler) {
  auto* p = &begin;
  while (p != &end) {
    CHECK(p);
    handler(p->data);
    p = p->next.get();
  }
}

}  // namespace spring

#endif  // SPRING_UTILS_H
