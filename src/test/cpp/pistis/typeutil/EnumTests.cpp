/** @file EnumTests.cpp
 *
 *  Unit tests for pistis::typeutil::Enum.
 */

#include <pistis/typeutil/Enum.hpp>
#include <gtest/gtest.h>
#include <sstream>

using namespace pistis::exceptions;
using namespace pistis::typeutil;

namespace {
  class TestEnum : public Enum<TestEnum> {
  public:
    static const TestEnum ONE;
    static const TestEnum TWO;
    static const TestEnum THREE;

  public:
    TestEnum(): Enum<TestEnum>(ONE) { }

  private:
    TestEnum(int value, const std::string& name):
      Enum(value, name) {
    }
  };

  const TestEnum TestEnum::ONE(1, "ONE");
  const TestEnum TestEnum::TWO(2, "TWO");
  const TestEnum TestEnum::THREE(3, "THREE");
}

TEST(EnumTests, Value) {
  EXPECT_EQ(TestEnum::ONE.value(), 1);
  EXPECT_EQ(TestEnum::TWO.value(), 2);
  EXPECT_EQ(TestEnum::THREE.value(), 3);
}

TEST(EnumTests, Name) {
  EXPECT_EQ(TestEnum::ONE.name(), "ONE");
  EXPECT_EQ(TestEnum::TWO.name(), "TWO");
  EXPECT_EQ(TestEnum::THREE.name(), "THREE");
}

TEST(EnumTests, Equality) {
  EXPECT_TRUE(TestEnum::ONE == TestEnum::ONE);
  EXPECT_FALSE(TestEnum::ONE == TestEnum::TWO);
}

TEST(EnumTests, Inequality) {
  EXPECT_TRUE(TestEnum::ONE != TestEnum::TWO);
  EXPECT_FALSE(TestEnum::ONE != TestEnum::ONE);
}

TEST(EnumTests, Comparison) {
  EXPECT_TRUE(TestEnum::ONE < TestEnum::TWO);
  EXPECT_FALSE(TestEnum::ONE < TestEnum::ONE);
  EXPECT_FALSE(TestEnum::TWO < TestEnum::ONE);

  EXPECT_TRUE(TestEnum::TWO > TestEnum::ONE);
  EXPECT_FALSE(TestEnum::TWO > TestEnum::TWO);
  EXPECT_FALSE(TestEnum::TWO > TestEnum::THREE);

  EXPECT_TRUE(TestEnum::TWO <= TestEnum::THREE);
  EXPECT_TRUE(TestEnum::TWO <= TestEnum::TWO);
  EXPECT_FALSE(TestEnum::THREE <= TestEnum::TWO);

  EXPECT_TRUE(TestEnum::THREE >= TestEnum::TWO);
  EXPECT_TRUE(TestEnum::THREE >= TestEnum::THREE);
  EXPECT_FALSE(TestEnum::ONE >= TestEnum::THREE);
}

TEST(EnumTests, Construction) {
  TestEnum defaultConstructed;
  TestEnum copied(TestEnum::TWO);

  EXPECT_EQ(defaultConstructed, TestEnum::ONE);
  EXPECT_EQ(copied, TestEnum::TWO);
}

TEST(EnumTests, Assignment) {
  TestEnum e;

  e= TestEnum::THREE;
  EXPECT_EQ(e, TestEnum::THREE);
}

TEST(EnumTests, FromValue) {
  EXPECT_EQ(TestEnum::fromValue(1), TestEnum::ONE);
  EXPECT_EQ(TestEnum::fromValue(2), TestEnum::TWO);
  EXPECT_EQ(TestEnum::fromValue(3), TestEnum::THREE);
  EXPECT_THROW(TestEnum::fromValue(4), NoSuchItem);
}

TEST(EnumTests, FromName) {
  EXPECT_EQ(TestEnum::fromName("ONE"), TestEnum::ONE);
  EXPECT_EQ(TestEnum::fromName("TWO"), TestEnum::TWO);
  EXPECT_EQ(TestEnum::fromName("THREE"), TestEnum::THREE);
  EXPECT_THROW(TestEnum::fromName("FOUR"), NoSuchItem);
}

TEST(EnumTests, Values) {
  const std::vector<TestEnum>& v= TestEnum::values();
  ASSERT_EQ(v.size(), 3);
  EXPECT_EQ(v[0], TestEnum::ONE);
  EXPECT_EQ(v[1], TestEnum::TWO);
  EXPECT_EQ(v[2], TestEnum::THREE);
}

TEST(EnumTests, Print) {
  std::ostringstream msg;
  msg << TestEnum::ONE << " " << TestEnum::TWO << " " << TestEnum::THREE;
  EXPECT_EQ(msg.str(), "ONE TWO THREE");
}

namespace {
  template <typename DerivedT, typename ImplT>
  class CustomEnumMemberData : public BasicEnumMemberData<DerivedT, ImplT> {
  public:
    virtual ~CustomEnumMemberData() { }

    virtual void add(ImplT* impl, const DerivedT& dv) {
      _auxToMember.insert(std::make_pair(impl->auxInfo(), dv));
      BasicEnumMemberData<DerivedT, ImplT>::add(impl, dv);
    }

    DerivedT fromAuxInfo(const std::string& auxInfo) {
      auto i= _auxToMember.find(auxInfo);
      if (i == _auxToMember.end()) {
	std::ostringstream msg;
	msg << "Member of " << nameOf<DerivedT>() << " with auxInfo \""
	    << auxInfo << "\"";
	throw NoSuchItem(msg.str(), PISTIS_EX_HERE);
      }
      return i->second;
    }
  private:
    std::map<std::string, DerivedT> _auxToMember;
  };

  class CustomEnumImpl : public BasicEnumImpl<int> {
  public:
    template <typename DerivedT, typename ImplT>
    struct MemberData {
      typedef CustomEnumMemberData<DerivedT, ImplT> type;
    };

  public:
    CustomEnumImpl(int value, const std::string& name,
		   const std::string& auxInfo):
      BasicEnumImpl<int>(value, name), _auxInfo(auxInfo) {
    }
  
    const std::string& auxInfo() const { return _auxInfo; }

    template <typename DerivedT, typename ImplT>
    static CustomEnumMemberData<DerivedT, ImplT>* createMemberData() {
      return new CustomEnumMemberData<DerivedT, ImplT>;
    }

  private:
    std::string _auxInfo;
  };

  class CustomEnum : public Enum<CustomEnum, CustomEnumImpl> {
  public:
    static const CustomEnum V1;
    static const CustomEnum V2;

  public:
    CustomEnum(): Enum<CustomEnum, CustomEnumImpl>(V1) { }

    const std::string& auxInfo() const { return _getImpl()->auxInfo(); }

    static CustomEnum fromAuxInfo(const std::string& auxInfo) {
      return _getMembers()->fromAuxInfo(auxInfo);
    }

  private:
    CustomEnum(int value, const std::string& name, const std::string& auxInfo):
      Enum<CustomEnum, CustomEnumImpl>(value, name, auxInfo) {
    }
  };

  const CustomEnum CustomEnum::V1(100, "V1", "AUX1");
  const CustomEnum CustomEnum::V2(101, "V2", "AUX2");
}

TEST(EnumTests, CustomEnum) {
  EXPECT_EQ(CustomEnum::V1.value(), 100);
  EXPECT_EQ(CustomEnum::V1.name(), "V1");
  EXPECT_EQ(CustomEnum::V1.auxInfo(), "AUX1");

  EXPECT_EQ(CustomEnum::V2.value(), 101);
  EXPECT_EQ(CustomEnum::V2.name(), "V2");
  EXPECT_EQ(CustomEnum::V2.auxInfo(), "AUX2");

  EXPECT_EQ(CustomEnum::fromName("V1"), CustomEnum::V1);
  EXPECT_EQ(CustomEnum::fromName("V2"), CustomEnum::V2);
  EXPECT_THROW(CustomEnum::fromName("NONE"), NoSuchItem);

  EXPECT_EQ(CustomEnum::fromValue(100), CustomEnum::V1);
  EXPECT_EQ(CustomEnum::fromValue(101), CustomEnum::V2);
  EXPECT_THROW(CustomEnum::fromValue(102), NoSuchItem);

  EXPECT_EQ(CustomEnum::fromAuxInfo("AUX1"), CustomEnum::V1);
  EXPECT_EQ(CustomEnum::fromAuxInfo("AUX2"), CustomEnum::V2);
  EXPECT_THROW(CustomEnum::fromAuxInfo("NONE"), NoSuchItem);
}

