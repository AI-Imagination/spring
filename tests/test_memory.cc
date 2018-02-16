#include "../memory.h"

#include <vector>
#include <gtest/gtest.h>


using namespace spring;
using namespace std;

TEST(memory, init) {
  auto vec = MakeShared<vector<int>>();
  vec->push_back(1);
  ASSERT_EQ(vec->size(), 1UL);
}
TEST(memory, init_with_args) {
  auto vec = MakeShared<vector<int>>(10, 0);
  ASSERT_EQ(vec->size(), 10UL);
}




