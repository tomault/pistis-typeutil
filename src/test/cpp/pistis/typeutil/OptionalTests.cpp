/** @file OptionalTests.cpp
 *
 *  Unit tests for pistis::typeutil::Optional
 */
#include <pistis/typeutil/Optional.hpp>
#include <gtest/gtest.h>
#include <ostream>
#include <sstream>

using namespace pistis::exceptions;
using namespace pistis::typeutil;

namespace {
  class Value {
  public:
    Value(): v_(0), moved_(false) { }
    explicit Value(int v): v_(v), moved_(false) { }
    Value(const Value& other): v_(other.v_), moved_(false) { }
    Value(Value&& other): v_(other.v_), moved_(true) { }

    int value() const { return v_; }
    bool moved() const { return moved_; }

    bool operator==(const Value& other) const { return v_ == other.v_; }
    bool operator==(int other) const { return v_ == other; }
    bool operator!=(const Value& other) const { return v_ != other.v_; }
    bool operator!=(int other) const { return v_ != other; }
  
    Value& operator=(const Value& other) {
      v_ = other.v_;
      moved_ = false;
      return *this;
    }

    Value& operator=(Value&& other) {
      v_ = other.v_;
      moved_ = true;
      return *this;
    }
  
  private:
    int v_;
    bool moved_;
  };

  inline bool operator==(int u, const Value& v) { return u == v.value(); }
  inline bool operator!=(int u, const Value& v) { return u != v.value(); }
  inline std::ostream& operator<<(std::ostream& out, const Value& v) {
    return out << v.value();
  }
}

TEST(OptionalTests, CreateEmptyOptional) {
  Optional<int> opt;

  EXPECT_TRUE(opt.empty());
  EXPECT_FALSE(opt.present());
}

TEST(OptionalTests, CreateOptionalFromCopyOfValue) {
  static const Value VALUE(10);
  Optional<Value> opt(VALUE);

  EXPECT_FALSE(opt.empty());
  EXPECT_TRUE(opt.present());
  EXPECT_EQ(VALUE, opt.value());
  EXPECT_FALSE(opt.value().moved());
}

TEST(OptionalTests, CreateOptionalByMovingValue) {
  Optional<Value> opt(Value(5));

  EXPECT_FALSE(opt.empty());
  EXPECT_TRUE(opt.present());
  EXPECT_EQ(5, opt.value());
  EXPECT_TRUE(opt.value().moved());
}

TEST(OptionalTests, ConstructFromCopy) {
  Optional<Value> src(Value(10));

  ASSERT_TRUE(!src.empty());
  ASSERT_TRUE(src.present());
  ASSERT_EQ(10, src.value());
  ASSERT_TRUE(src.value().moved());

  Optional<Value> copy(src);

  EXPECT_FALSE(copy.empty());
  EXPECT_TRUE(copy.present());
  EXPECT_EQ(10, copy.value());
  EXPECT_FALSE(copy.value().moved());
}

TEST(OptionalTests, ConstructByCopyingOtherType) {
  Optional<int> opt(22);
  Optional<Value> copy(opt);

  EXPECT_FALSE(copy.empty());
  EXPECT_TRUE(copy.present());
  EXPECT_EQ(22, copy.value());
  EXPECT_FALSE(copy.value().moved());
}

TEST(OptionalTests, ConstructByMove) {
  static const Value VALUE(8);
  Optional<Value> src(VALUE);

  ASSERT_TRUE(!src.empty());
  ASSERT_TRUE(src.present());
  ASSERT_EQ(8, src.value());
  ASSERT_TRUE(!src.value().moved());
  
  Optional<Value> copy(std::move(src));
  EXPECT_FALSE(copy.empty());
  EXPECT_TRUE(copy.present());
  EXPECT_EQ(8, copy.value());
  EXPECT_TRUE(copy.value().moved());
  EXPECT_TRUE(src.empty());  // Move leaves source empty
}

TEST(OptionalTests, AssignCopy) {
  Optional<Value> empty;
  Optional<Value> opt(Value(9));
  Optional<Value> copy;

  ASSERT_TRUE(empty.empty());
  ASSERT_TRUE(opt.present());
  ASSERT_EQ(9, opt.value());
  ASSERT_TRUE(opt.value().moved());
  ASSERT_TRUE(copy.empty());

  copy = opt;
  ASSERT_TRUE(copy.present());
  EXPECT_EQ(9, copy.value());
  EXPECT_FALSE(copy.value().moved());

  // src was not changed
  ASSERT_TRUE(opt.present());
  EXPECT_EQ(9, opt.value());
  EXPECT_TRUE(opt.value().moved());

  copy = empty;
  EXPECT_TRUE(copy.empty());
  EXPECT_TRUE(empty.empty());
}

TEST(OptionalTests, AssignOptionalOfOtherType) {
  Optional<int> empty;
  Optional<int> opt(11);
  Optional<Value> copy;

  ASSERT_TRUE(empty.empty());
  ASSERT_TRUE(opt.present());
  ASSERT_EQ(11, opt.value());
  ASSERT_TRUE(copy.empty());

  copy = opt;
  ASSERT_TRUE(copy.present());
  EXPECT_EQ(11, copy.value());
  EXPECT_FALSE(copy.value().moved());

  // src was not changed
  ASSERT_TRUE(opt.present());
  EXPECT_EQ(11, opt.value());

  copy = empty;
  EXPECT_TRUE(copy.empty());
  EXPECT_TRUE(empty.empty());
}

TEST(OptionalTests, AssignByMove) {
  static const Value VALUE(9);
  Optional<Value> opt(VALUE);
  Optional<Value> copy;

  ASSERT_TRUE(opt.present());
  ASSERT_EQ(9, opt.value());
  ASSERT_TRUE(!opt.value().moved());
  ASSERT_TRUE(copy.empty());

  copy = std::move(opt);
  ASSERT_TRUE(copy.present());
  EXPECT_EQ(9, copy.value());
  EXPECT_TRUE(copy.value().moved());

  EXPECT_TRUE(opt.empty());  // Moving leaves src empty
}

TEST(OptionalTests, AccessValue) {
  Optional<int> empty;
  Optional<int> opt(5);
  const Optional<int>& constEmpty(empty);
  const Optional<int>& constOpt(opt);

  EXPECT_THROW(constEmpty.value(), OptionalEmptyError);
  EXPECT_THROW(empty.value(), OptionalEmptyError);
  EXPECT_EQ(5, constOpt.value());
  EXPECT_EQ(5, opt.value());
}

TEST(OptionalTests, ValueOr) {
  Optional<int> empty;
  Optional<int> opt(6);

  EXPECT_EQ(6, opt.valueOr(10));
  EXPECT_EQ(10, empty.valueOr(10));
}

TEST(OptionalTests, ValueOrCall) {
  Optional<int> empty;
  Optional<int> opt(6);

  EXPECT_EQ(6, opt.valueOrCall([]() { return -1; }));
  EXPECT_EQ(-1, empty.valueOrCall([]() { return -1; }));
}

TEST(OptionalTests, IfPresent) {
  Optional<int> empty;
  Optional<int> opt(3);
  int numCalls = 0;
  int recoveredValue = 0;
  auto action = [&numCalls, &recoveredValue](int v) {
    ++numCalls;
    recoveredValue = v;
  };

  opt.ifPresent(action);
  EXPECT_EQ(1, numCalls);
  EXPECT_EQ(3, recoveredValue);

  empty.ifPresent(action);
  EXPECT_EQ(1, numCalls);
  EXPECT_EQ(3, recoveredValue);  
}

TEST(OptionalTests, OrElse) {
  Optional<int> empty;
  Optional<int> opt(3);
  int numCalls = 0;
  auto action = [&numCalls]() { ++numCalls; };

  opt.orElse(action);
  EXPECT_EQ(0, numCalls);

  empty.orElse(action);
  EXPECT_EQ(1, numCalls);
}

TEST(OptionalTests, IfPresentOrElse) {
  Optional<int> empty;
  Optional<int> opt(99);
  int numIfCalls = 0;
  int recoveredValue = 0;
  int numElseCalls = 0;
  auto ifAction = [&numIfCalls, &recoveredValue](int v) {
    ++numIfCalls;
    recoveredValue = v;
  };
  auto elseAction = [&numElseCalls]() { ++numElseCalls; };

  opt.ifPresent(ifAction).orElse(elseAction);
  EXPECT_EQ(1, numIfCalls);
  EXPECT_EQ(99, recoveredValue);
  EXPECT_EQ(0, numElseCalls);

  numIfCalls = 0;
  recoveredValue = 0;

  empty.ifPresent(ifAction).orElse(elseAction);
  EXPECT_EQ(0, numIfCalls);
  EXPECT_EQ(0, recoveredValue);
  EXPECT_EQ(1, numElseCalls);
}

TEST(OptionalTests, Map) {
  Optional<int> empty;
  Optional<int> opt(4);
  auto action = [](int v) -> std::string {
    std::ostringstream tmp;
    tmp << v;
    return tmp.str();
  };

  EXPECT_EQ("4", opt.map(action).value());
  EXPECT_TRUE(empty.map(action).empty());
}

TEST(OptionalTests, Apply) {
  Optional<int> empty;
  Optional<int> opt(4);
  int numCalls = 0;
  auto action = [&numCalls](int v) -> std::string {
    std::ostringstream tmp;
    tmp << v;
    ++numCalls;
    return tmp.str();
  };

  EXPECT_EQ("4", opt.apply(action));
  EXPECT_EQ(1, numCalls);
  
  EXPECT_EQ("", empty.apply(action));
  EXPECT_EQ(1, numCalls);
}

TEST(OptionalTests, ApplyOr) {
  Optional<int> empty;
  Optional<int> opt(4);
  auto presentAction = [](int v) -> std::string {
    std::ostringstream tmp;
    tmp << v;
    return tmp.str();
  };
  auto absentAction = []() -> const char* { return "no_value"; };

  EXPECT_EQ("4", opt.applyOr(presentAction, absentAction));
  EXPECT_EQ("no_value", empty.applyOr(presentAction, absentAction));
}

TEST(OptionalTests, Filter) {
  Optional<int> empty;
  Optional<int> opt10(10);
  Optional<int> opt1(1);
  auto greaterThan5 = [](int v) -> bool { return v > 5; };

  EXPECT_EQ(10, opt10.filter(greaterThan5).value());
  EXPECT_TRUE(opt1.filter(greaterThan5).empty());
  EXPECT_TRUE(empty.filter(greaterThan5).empty());
}

TEST(OptionalTests, Clear) {
  Optional<int> empty;
  Optional<int> opt(21);

  ASSERT_TRUE(empty.empty());
  ASSERT_TRUE(opt.present());
  ASSERT_EQ(21, opt.value());

  opt.clear();
  EXPECT_TRUE(opt.empty());

  empty.clear();
  EXPECT_TRUE(opt.empty());
}

TEST(OptionalTests, ConvertToBool) {
  Optional<int> empty;
  Optional<int> opt(21);

  EXPECT_FALSE(empty);
  EXPECT_TRUE(opt);
}

TEST(OptionalTests, Equality) {
  Optional<int> empty;
  Optional<int> alsoEmpty;
  Optional<int> five(5);
  Optional<int> alsoFive(5);
  Optional<int> seven(7);

  EXPECT_TRUE(empty == alsoEmpty);
  EXPECT_TRUE(five == alsoFive);
  EXPECT_FALSE(empty == five);
  EXPECT_FALSE(five == empty);
  EXPECT_FALSE(five == seven);
}

TEST(OptionalTests, Inequality) {
  Optional<int> empty;
  Optional<int> alsoEmpty;
  Optional<int> five(5);
  Optional<int> alsoFive(5);
  Optional<int> seven(7);

  EXPECT_TRUE(empty != five);
  EXPECT_TRUE(five != empty);
  EXPECT_TRUE(five != seven);
  EXPECT_FALSE(empty != alsoEmpty);
  EXPECT_FALSE(five != alsoFive);
}

