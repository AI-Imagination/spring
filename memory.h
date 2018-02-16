#ifndef SPRING_MEMORY_H
#define SPRING_MEMORY_H

#include <cstdint>
#include <iostream>

namespace spring {

template <typename D>
struct DefaultAllocater {
  static DefaultAllocater<D> &Global() {
    static DefaultAllocater<D> _;
    return _;
  }
  template <typename... Args>
  D *operator()(Args... args) {
    return new D(args...);
  }
};

template <typename D>
struct DefaultDeletor {
  static DefaultDeletor<D> &Global() {
    static DefaultDeletor<D> _;
    return _;
  }
  void operator()(D *ptr) { delete ptr; }
};

// Simple shared pointer implementation, not thread-safe, but lighter to use.
template <typename T, typename Allocater = DefaultAllocater<T>,
          typename Delector = DefaultDeletor<T>>
class SharedPtr {
 public:
  using value_t = T;
  using self_t = SharedPtr<T, Allocater, Delector>;

  struct Aux {
    unsigned int count{0};
    unsigned int inc() { return ++count; }
    unsigned int dec() { return --count; }
  };

  SharedPtr() = default;
  SharedPtr(const SharedPtr &other) {
    aux_ = other.aux_;
    pt_ = other.pt_;
    aux_->inc();
  }
  SharedPtr(value_t *pt) : pt_(pt), aux_(new Aux) { aux_->inc(); }
  SharedPtr(SharedPtr &&other) {
    pt_ = other.pt_;
    aux_ = other.aux_;
  }
  SharedPtr &operator=(const SharedPtr &other) {
    aux_ = other.aux_;
    pt_ = other.pt_;
    aux_->inc();
  }
  SharedPtr &operator=(SharedPtr &&other) {
    aux_ = other.aux_;
    pt_ = other.pt_;
  }

  value_t &operator*() { return *pt_; }
  value_t *operator->() { return pt_; }
  const value_t *get() { return pt_; }
  explicit operator bool() { return get() != nullptr; }

  ~SharedPtr() {
    if (aux_->dec() == 0) {
      Delector::Global()(pt_);
    }
  }

 private:
  Aux *aux_;
  value_t *pt_{nullptr};
};

template <typename T, typename... Args,
          typename Allocater = DefaultAllocater<T>,
          typename Deletor = DefaultDeletor<T>>
SharedPtr<T> MakeShared(Args... args) {
  auto *pt = Allocater::Global()(args...);
  return SharedPtr<T>(pt);
};

}  // namespace spring

#endif  // SPRING_MEMORY_H
