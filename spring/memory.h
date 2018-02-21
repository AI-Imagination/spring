#ifndef SPRING_MEMORY_H
#define SPRING_MEMORY_H

#include <glog/logging.h>
#include <cstdint>
#include <iostream>

namespace spring {

struct Memory {
  static Memory &Global() {
    static Memory _;
    return _;
  }

  size_t size() const { return size_; }

  template <typename D, typename... Args>
  D *New(Args... args) {
    size_ += sizeof(D);
    return new D(args...);
  }

  template <typename D>
  void Del(D *p) {
    size_ -= sizeof(*p);
    delete (p);
  }

  friend std::ostream &operator<<(std::ostream &os, const Memory &other) {
    os << "<Memory size:" << other.size() << ">";
    return os;
  }

 private:
  size_t size_{0};
};

// Simple shared pointer implementation, not thread-safe, but lighter to use.
template <typename T, typename MemoryHandler = Memory>
class SharedPtr {
 public:
  using value_t = T;
  using self_t = SharedPtr<T, MemoryHandler>;

  struct Aux {
    unsigned int count{0};
    unsigned int inc() { return ++count; }
    unsigned int dec() { return --count; }
  };

#define DEC_DEL                   \
  if (aux_ && aux_->dec() == 0) { \
    del();                        \
  }

  SharedPtr() = default;
  SharedPtr(const SharedPtr &other) {
    if (*this == other) return;
    if (!other) {
      Reset();
      return;
    }
    DEC_DEL
    aux_ = other.aux_;
    pt_ = other.pt_;
    if (aux_) {
      CHECK(pt_);
      aux_->inc();
    }
  }
  SharedPtr(value_t *pt) { Reset(pt); }
  SharedPtr(SharedPtr &&other) {
    if (*this == other) return;
    DEC_DEL
    pt_ = other.pt_;
    aux_ = other.aux_;
    other.pt_ = nullptr;
    other.aux_ = nullptr;
  }
  SharedPtr &operator=(const SharedPtr &other) {
    if (*this != other) {
      DEC_DEL
      aux_ = other.aux_;
      pt_ = other.pt_;
      if (aux_) {
        CHECK(pt_);
        aux_->inc();
      }
    }
    return *this;
  }
  SharedPtr &operator=(SharedPtr &&other) {
    if (*this != other) {
      DEC_DEL
      aux_ = other.aux_;
      pt_ = other.pt_;
      other.pt_ = nullptr;
      other.aux_ = nullptr;
    }
    return *this;
  }

  void Reset(value_t *pt = nullptr) {
    if (*this == pt) return;
    DEC_DEL
    aux_ = nullptr;
    if (pt) {
      aux_ = new Aux;
      aux_->inc();
    }
    pt_ = pt;
  }

  value_t &operator*() const { return *pt_; }
  value_t *operator->() const { return pt_; }
  bool operator==(const self_t &other) const { return pt_ == other.pt_; }
  bool operator==(const T *other) const { return pt_ == other; }
  bool operator!=(const self_t &other) const { return pt_ != other.pt_; }
  explicit operator bool() const { return pt_ != nullptr; }

  value_t *const get() const { return pt_; }
  const Aux *aux() const { return aux_; }

  ~SharedPtr() { DEC_DEL }

 private:
  void del() {
    if (aux_) {
      CHECK_EQ(aux_->count, 0);
      delete aux_;
      MemoryHandler::Global().Del(pt_);
      aux_ = nullptr;
      pt_ = nullptr;
    }
  }
  Aux *aux_{nullptr};
  value_t *pt_{nullptr};
};

template <typename T, typename... Args, typename MemoryHandler = Memory>
SharedPtr<T> MakeShared(Args... args) {
  T *pt = MemoryHandler::Global().template New<T, Args...>(args...);
  return SharedPtr<T>(pt);
};

}  // namespace spring

#endif  // SPRING_MEMORY_H
