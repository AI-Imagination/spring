#ifndef SPRING_UTILS_H
#define SPRING_UTILS_H

#include <glog/logging.h>
#include <functional>

#include "memory.h"

namespace spring {

#define __repr__(Cls__) \
  friend std::ostream& operator<<(std::ostream& os, const Cls__& self)

template <typename T>
struct List {
  struct Node {
    Node(const T& data) : data(data) {}

    T data;
    SharedPtr<Node> next;
    Node* pre{nullptr};  // raw pointer to avoid cyclic reference.

    __repr__(List<T>) {
      os << self.data;
      return os;
    }
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

  const T& Front() const { return head->data; }
  const T& Back() const { return tail->data; }

  node_ptr PushFront(const T& data) {
    auto n = MakeShared<Node>(data);
    if (head) {
      head->pre = n.get();
      n->next = head;
      head = n;
    } else {
      head = n;
      tail = n;
    }
    return n;
  }

  void PushBack(const T& data) {
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

  node_ptr InsertAfter(const node_ptr& pre, const T& n) {
    CHECK(pre);
    auto node = MakeShared<Node>(n);
    node->pre = pre.get();
    node->next = pre->next;
    pre->next = node;
    return node;
  }
  node_ptr InsertAfter(Node* pre, const T& n) {
    CHECK(pre);
    auto node = MakeShared<Node>(n);
    node->pre = const_cast<Node*>(pre);
    node->next = pre->next;
    pre->next = node;
    return node;
  }
  node_ptr Insert(const node_ptr& node, const T& n) {
    if (node && node->pre) {
      auto x = MakeShared<Node>(n);
      x->pre = node->pre;
      if (x->pre) x->pre->next = x;
      x->next = node;
      node->pre = x.get();
      return x;
    } else {
      return PushFront(n);
    }
  }
  void PopFront() {
    CHECK(!Empty());
    if (head == tail) {
      head.Reset();
      tail.Reset();
    } else {
      CHECK(head->next);
      // the head->next will be deleted first, hold it.
      auto p = head->next;
      head = p;
      CHECK(head);
      head->pre = nullptr;
    }
  }

  void PopBack() {
    CHECK(!Empty());
    if (head == tail) {
      head.Reset();
      tail.Reset();
    } else {
      if (tail->pre->pre) {
        auto x = tail->pre->pre->next;
        tail = x;
      } else {
        tail = head;
      }
    }
    if (tail) tail->next = nullptr;
  }

  void Remove(node_ptr n) {
    if (Empty()) return;
    if (!n) return;
    if (n == head) {
      PopFront();
    } else if (n == tail) {
      PopBack();
    } else {
      n->next->pre = n->pre;
      n->pre->next = n->next;
    }
  }

  // Remove the nodes between (begin_pre+1, end) inclusively.
  template <typename Ptr>
  void Remove(Ptr begin, Ptr end) {
    CHECK(begin);
    CHECK(end);
    if (begin == end) {
      Remove(begin);
    } else if (!begin->pre || !end->next) {  // head or tail
      if (!begin->pre) {
        // begin is head
        CHECK(head == begin);
        head = end->next;
      }
      if (!end->next) {
        // end is tail
        CHECK(tail == end);
        if (begin->pre) {
          tail = begin->pre->pre ? begin->pre->pre->next : head;
        } else {
          tail = head;
        }
      }
    } else {
      begin->pre->next = end->next;
      if (end->next) {
        end->next->pre = begin->pre;
      }
    }
    if (head) head->pre = nullptr;
    if (tail) tail->next.Reset();
  }

  void RemoveIf(const std::function<bool(const T&)>& tester) {
    // TODO update tail
    if (Empty()) return;
    auto p = head;
    while (p) {
      if (tester(p->data)) {
        Remove(p);
      }
      p = p->next;
    }
  }

  bool Empty() const { return !bool(head); }

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

namespace debug {

template <typename D>
void ValidList(const List<D>& list) {
  if (list.Empty()) return;
  CHECK(list.tail);
  CHECK(!list.head->pre);
  CHECK(!list.tail->next);
  auto p = list.head;
  while (p) {
    if (!p->next) {
      CHECK(p == list.tail);
    } else {
      CHECK(p == p->next->pre);
    }
    p = p->next;
  }
}

}  // namespace debug

}  // namespace spring

#endif  // SPRING_UTILS_H
