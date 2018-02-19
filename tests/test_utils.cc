#include "spring/utils.h"

#include <gtest/gtest.h>

using namespace spring;

class ListTester : public ::testing::Test {
 public:
  virtual void SetUp() override {
    for (int i = 0; i < 10; i++) {
      list.PushBack(i);
    }
    debug::ValidList(list);
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
TEST_F(ListTester, PopFront) {
  ASSERT_EQ(list.heavy_size(), 10);
  ASSERT_EQ(list.Front(), 0);
  ASSERT_EQ(list.Back(), 9);
  ASSERT_TRUE(list.head);
  ASSERT_TRUE(list.head->next);
  list.PopFront();
  EXPECT_EQ(list.heavy_size(), 9);
  EXPECT_EQ(list.Front(), 1);
}
TEST_F(ListTester, PopBack) {
  list.PopBack();
  debug::ValidList(list);
  EXPECT_EQ(list.Back(), 8);
  EXPECT_EQ(list.heavy_size(), 9);
}
TEST_F(ListTester, Remove) {
  list.Remove(list.head);
  EXPECT_EQ(list.head->data, 1);
  EXPECT_EQ(list.head->pre, nullptr);
  EXPECT_EQ(list.head->next->data, 2);
  debug::ValidList(list);
}
TEST_F(ListTester, Remove_head) {
  auto begin = list.head;
  list.Remove(begin);
  EXPECT_EQ(list.heavy_size(), 9);
  debug::ValidList(list);
}
TEST_F(ListTester, Remove_tail) {
  auto tail = list.tail;
  list.Remove(tail);
  EXPECT_EQ(list.heavy_size(), 9);
  debug::ValidList(list);
}
TEST_F(ListTester, Remove_head_sublist) {
  auto tail = list.head->next->next->next;
  auto head = list.head;
  list.Remove(head, tail);
  debug::ValidList(list);
  EXPECT_EQ(list.heavy_size(), 6);
}
TEST_F(ListTester, Remove_tail_sublist) {
  auto head = list.head->next->next;
  auto tail = list.tail;
  list.Remove(head, tail);
  debug::ValidList(list);
  EXPECT_EQ(list.heavy_size(), 2);
}
/*
TEST_F(ListTester, RemoveIf) {
  list.RemoveIf([](const int& v) { return v > 7; });
  EXPECT_EQ(list.heavy_size(), 8);
  debug::ValidList(list);
}
*/
TEST_F(ListTester, operator_at) {
  for (int i = 0; i < 10; i++) {
    EXPECT_EQ(list[i], i);
  }
}
TEST(List, PushFront) {
  List<int> list;
  for (int i = 0; i < 10; i++) {
    list.PushFront(i);
  }

  ASSERT_EQ(list.heavy_size(), 10);

  auto* p = list.head.get();
  ASSERT_TRUE(p);

  int count = 9;
  while (p) {
    EXPECT_EQ(p->data, count--);
    p = p->next.get();
  }
  debug::ValidList(list);
}
TEST_F(ListTester, Insert) {
  auto n = list.Insert(nullptr, 100);
  ASSERT_EQ(list.heavy_size(), 11);
  ASSERT_EQ(list.head->data, 100);

  list.Insert(n, 101);
  ASSERT_EQ(list.heavy_size(), 12);
  ASSERT_EQ(list.head->data, 101);

  list.Insert(n, 102);
  ASSERT_EQ(list.heavy_size(), 13);
  ASSERT_EQ(list.head->next->data, 102);
  debug::ValidList(list);
}
