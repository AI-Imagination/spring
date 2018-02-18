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
    if (aux_) {
      aux_->dec();
    }
    aux_ = other.aux_;
    pt_ = other.pt_;
    if (aux_) {
      aux_->inc();
    }
  }
  SharedPtr(value_t *pt) { Reset(pt); }
  SharedPtr(SharedPtr &&other) {
    pt_ = other.pt_;
    aux_ = other.aux_;
    other.pt_ = nullptr;
    other.aux_ = nullptr;
  }
  SharedPtr &operator=(const SharedPtr &other) {
    aux_ = other.aux_;
    pt_ = other.pt_;
    if (aux_) aux_->inc();
    return *this;
  }
  SharedPtr &operator=(SharedPtr &&other) {
    aux_ = other.aux_;
    pt_ = other.pt_;
    other.pt_ = nullptr;
    other.aux_ = nullptr;
    return *this;
  }

  void Reset(value_t *pt = nullptr) {
    if (aux_ && aux_->dec() == 0) {
      delete aux_;
      Delector::Global()(pt_);
      aux_ = nullptr;
      pt_ = nullptr;
    }
    if (pt != nullptr) {
      aux_ = new Aux;
      aux_->inc();
    }
    pt_ = pt;
  }
  value_t &operator*() const { return *pt_; }
  value_t *operator->() const { return pt_; }
  bool operator==(const self_t &other) const { return pt_ == other.pt_; }
  bool operator==(const T* other) const { return pt_ == other; }
  bool operator!=(const self_t &other) const { return pt_ != other.pt_; }
  explicit operator bool() const { return pt_ != nullptr; }

  value_t *const get() const { return pt_; }
  const Aux *aux() const { return aux_; }

  ~SharedPtr() {
    if (aux_ && aux_->dec() == 0) {
      delete aux_;
      Delector::Global()(pt_);
      aux_ = nullptr;
      pt_ = nullptr;
    }
  }

 private:
  Aux *aux_{nullptr};
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
