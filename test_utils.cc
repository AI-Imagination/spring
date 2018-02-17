#include "utils.h"

#include <gtest/gtest.h>

using namespace spring;

TEST(List, create) {
  List<int> list;
  for (int i = 0; i < 10; i++) {
    list.Append(i);
  }
}
