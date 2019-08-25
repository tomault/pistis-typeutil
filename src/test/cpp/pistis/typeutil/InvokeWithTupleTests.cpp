#include <pistis/typeutil/InvokeWithTuple.hpp>
#include <gtest/gtest.h>
#include <sstream>
#include <string>

using namespace pistis::typeutil;

namespace {
  void g(int x, const char* y) {

  }
}

TEST(InvokeWithTupleTests, Invoke) {
  auto f = [](const std::string& x, int y) {
    std::ostringstream msg;
    msg << "x = " << x << ", y = " << y;
    return msg.str();
  };
  auto args = std::make_tuple(std::string("hello"), (int)10);

  EXPECT_EQ("x = hello, y = 10", invokeWithTuple(f, args));
  EXPECT_EQ("x = a, y = 1", invokeWithTuple(f, std::make_tuple("a", 1)));
}

TEST(InvokeWithTupleTests, InvokeOnVoid) {
  auto args = std::make_tuple(10, "world");
  invokeWithTuple(g, args);
}
