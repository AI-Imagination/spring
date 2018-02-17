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
TEST(memory, move) {
  using ptr = SharedPtr<vector<int>>;
  {
    auto vec = MakeShared<vector<int>>();
    auto vec1 = std::move(vec);
    EXPECT_EQ(vec1.aux()->count, 1);
    auto vec2 = std::move(vec1);
    EXPECT_EQ(vec2.aux()->count, 1);
  }
  {
    auto vec = MakeShared<vector<int>>();
    // call copy construction
    ptr vec1(vec);
    EXPECT_EQ(vec1.aux()->count, 2);

    // call move construction
    ptr vec2(std::move(vec));
    EXPECT_EQ(vec2.aux()->count, 2);
    EXPECT_EQ(vec.get(), nullptr);
  }
}
TEST(memory, ref_count) {
  auto vec = MakeShared<vector<int>>();
  EXPECT_EQ(vec.aux()->count, 1);
  auto vec1 = vec;
  EXPECT_EQ(vec.aux()->count, 2);
  EXPECT_EQ(vec1.aux()->count, 2);

  {
    auto vec3 = vec;
    EXPECT_EQ(vec.aux()->count, 3);
    EXPECT_EQ(vec1.aux()->count, 3);
  }
  EXPECT_EQ(vec.aux()->count, 2);
}
TEST(memory, bool_op) {
  using ptr = SharedPtr<vector<int>> ;
  ptr epy;
  EXPECT_FALSE(epy);

  ptr tre = MakeShared<vector<int>>();
  EXPECT_TRUE(tre);
}
// TODO(Superjomn) create a benchmark to compare the performance with std::shared_ptr




