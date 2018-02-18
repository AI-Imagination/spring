#include "spring/utils.h"

#include <gtest/gtest.h>

using namespace spring;

class ListTester : public ::testing::Test {
 public:
  virtual void SetUp() override {
    for (int i = 0; i < 10; i++) {
      list.Append(i);
    }
  }

  List<int> list;
};

TEST_F(ListTester, create) { EXPECT_EQ(list.heavy_size(), 10); }
TEST_F(ListTester, Visit) {
  int count = 0;
  Visit<int>(*list.head, *list.tail, [&](const int& v) {
    EXPECT_EQ(v, count);
    count++;
  });
}
TEST_F(ListTester, RemoveIf) {
  list.RemoveIf([](const int& v) { return v > 7; });
  EXPECT_EQ(list.heavy_size(), 8);
}
TEST_F(ListTester, Remove) {
  list.Remove(list.head);
  EXPECT_EQ(list.head->data, 1);
  EXPECT_EQ(list.head->pre, nullptr);
  EXPECT_EQ(list.head->next->data, 2);
}
TEST_F(ListTester, operator_at) {
  for (int i = 0; i < 10; i++) {
    EXPECT_EQ(list[i], i);
  }
}
