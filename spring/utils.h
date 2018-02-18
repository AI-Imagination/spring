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
    Node* pre{nullptr};  // raw pointer to avoid cyclic reference.
  };

  using node_ptr = SharedPtr<Node>;

  List() = default;

  List(const node_ptr& begin, const node_ptr& end) : head(begin), tail(end) {}

  const T& operator[](size_t offset) {
    auto* p = head.get();
    for (size_t i = 0; i < offset; i++) {
      p = p->next.get();
    }
    return p->data;
  }

  void Append(const T& data) {
    auto n = MakeShared<Node>(data);
    if (!head) {
      head = n;
      tail = n;
    } else {
      CHECK(tail);
      n->pre = tail.get();
      tail->next = n;
      tail = n;
    }
  }

  void InsertAfter(const node_ptr& pre, const T& n) {
    auto node = MakeShared<Node>(n);
    node->pre = pre.get();
    node->next = pre->next;
    pre->next = node;
  }
  void InsertAfter(Node* pre, const T& n) {
    auto node = MakeShared<Node>(n);
    node->pre = const_cast<Node*>(pre);
    node->next = pre->next;
    pre->next = node;
  }

  void PushFront(const T& data) {
    auto n = MakeShared<Node>(data);
    head->pre = n.get();
    if (head) {
      n->next = head;
    }
    head = n;
  }

  void PopFront() {
    if (!head) return;
    head = head->next;
    head->pre = nullptr;
  }

  void Remove(node_ptr n) {
    if (!n) return;
    ;
    if (n == head) {
      if (head->next) {
        head->next->pre = nullptr;
        head = head->next;
      } else {
        head = nullptr;
        tail = nullptr;
      }
    } else {
      n->next->pre = n->pre;
      n->pre->next = n->next;
    }
  }

  // Remove the nodes between (begin_pre+1, end) inclusively.
  template <typename Ptr>
  void Remove(Ptr begin, Ptr end) {
    if (head == begin) {
      if (end) {
        head = end->next;
        end->pre = nullptr;
      }
    }
    if (tail == end) {
      if (!begin->pre) {
        tail.Reset();
      }
    } else {
      begin->pre->next = end->next;
      end->next->pre = begin->pre;
    }
  }

  void RemoveIf(const std::function<bool(const T&)>& tester) {
    // TODO update tail
    if (!head) return;
    auto p = head->next;
    while (p) {
      if (tester(p->data)) {
        if (p->next) {
          p->next->pre = p->pre;
          p->pre->next = p->next;
        } else {  // tail
          p->pre->next.Reset();
        }
      }
      p = p->next;
    }
    if (tester(head->data)) {
      head = head->next;
      head->pre = nullptr;
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
