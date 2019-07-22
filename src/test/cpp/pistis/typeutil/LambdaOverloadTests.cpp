#include <pistis/typeutil/LambdaOverload.hpp>
#include <gtest/gtest.h>
#include <string>

using namespace pistis::typeutil;

namespace {
  template <typename Function, typename... Value>
  auto invoke(Function f, Value... v) { return f(v...); }

  auto makeOverload() {
    return overloadLambda([](int v) { return -v; },
			  [](const std::string& s) { return s + ".moo"; },
			  [](int x, int y) { return 2 * x + y; });
  }
}

TEST(LambdaOverloadTests, InvokeOnInteger) {
  EXPECT_EQ(-1, invoke(makeOverload(), (int)1));
}

TEST(LambdaOverloadTests, InvokeOnString) {
  EXPECT_EQ("cow.moo", invoke(makeOverload(), "cow"));
}

TEST(LambdaOverloadTests, InvokeWithTwoArgs) {
  EXPECT_EQ(7, invoke(makeOverload(), 2, 3));
}

