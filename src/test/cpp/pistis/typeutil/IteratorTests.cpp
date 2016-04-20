/** @file IteratorTests.cpp
 *
 *  Unit tests for the iterator type generators in
 *  pistis/typeutil/Iterators.hpp
 *
 *  @TODO  Use the iterator testing library in pistis::testing instead
 *         of the custom test code in this file.
 */

#include <pistis/typeutil/Iterators.hpp>
#include <gtest/gtest.h>
#include <initializer_list>
#include <iostream>
#include <sstream>
#include <vector>

namespace {
  typedef std::vector<int> IntVectorType;
  typedef IntVectorType::iterator STLIterator;

  template <typename DataT>
  class ConstCustomIteratorImpl {
  public:
    ConstCustomIteratorImpl(): _p(nullptr), _moved(false) { }
    ConstCustomIteratorImpl(const DataT* p): _p(p), _moved(false) { }
    ConstCustomIteratorImpl(const ConstCustomIteratorImpl& other):
        _p(other._p), _moved(false) {
    }
    ConstCustomIteratorImpl(ConstCustomIteratorImpl&& other):
        _p(other._p), _moved(true) {
    }

    const DataT* ptr() const { return _p; }
    bool moved() const { return _moved; }
    const DataT& operator*() const { return *_p; }
    const DataT* operator->() const { return _p; }
    const DataT& operator[](ptrdiff_t i) const { return _p[i]; }
    void operator++() { ++_p; }
    void operator--() { --_p; }
    ConstCustomIteratorImpl operator+(ptrdiff_t d) const {
      return ConstCustomIteratorImpl(_p + d);
    }
    ConstCustomIteratorImpl operator-(ptrdiff_t d) const {
      return ConstCustomIteratorImpl(_p - d);
    }
    ptrdiff_t operator-(const ConstCustomIteratorImpl& other) const {
      return _p - other._p;
    }
    void operator+=(ptrdiff_t d) { _p += d; }
    void operator-=(ptrdiff_t d) { _p -= d; }

    ConstCustomIteratorImpl& operator=(const ConstCustomIteratorImpl& other) {
      _p= other._p;
      _moved= false;
      return *this;
    }

    ConstCustomIteratorImpl& operator=(ConstCustomIteratorImpl&& other) {
      _p= other._p;
      _moved= true;
      return *this;
    }

    bool operator==(const ConstCustomIteratorImpl& other) const {
      return _p == other._p;
    }
    bool operator!=(const ConstCustomIteratorImpl& other) const {
      return _p != other._p;
    }
    bool operator<(const ConstCustomIteratorImpl& other) const {
      return _p < other._p;
    }
    bool operator<=(const ConstCustomIteratorImpl& other) const {
      return _p <= other._p;
    }
    bool operator>=(const ConstCustomIteratorImpl& other) const {
      return _p >= other._p;
    }
    bool operator>(const ConstCustomIteratorImpl& other) const {
      return _p > other._p;
    }
  
  protected:
    const DataT* _p;
    bool _moved;
  };

  template <typename DataT>
  class CustomIteratorImpl : public ConstCustomIteratorImpl<DataT> {
  public:
    CustomIteratorImpl(): ConstCustomIteratorImpl<DataT>() { }
    CustomIteratorImpl(DataT* p): ConstCustomIteratorImpl<DataT>(p) { }
    CustomIteratorImpl(const CustomIteratorImpl& other):
        ConstCustomIteratorImpl<DataT>(other) {
    }
    CustomIteratorImpl(CustomIteratorImpl&& other):
        ConstCustomIteratorImpl<DataT>(std::move(other)) {
    }

    DataT* ptr() const { return const_cast<DataT*>(this->_p); }
    DataT& operator*() const { return *ptr(); }
    DataT* operator->() const { return ptr(); }
    DataT& operator[](ptrdiff_t i) const { return *(ptr()+i); }
    CustomIteratorImpl operator+(ptrdiff_t d) const {
      return CustomIteratorImpl<DataT>(ptr() + d);
    }
    CustomIteratorImpl operator-(ptrdiff_t d) const {
      return CustomIteratorImpl<DataT>(ptr() - d);
    }
    ptrdiff_t operator-(const CustomIteratorImpl& other) const {
      return ptr() - other.ptr();
    }

    CustomIteratorImpl& operator=(const CustomIteratorImpl& other) {
      ConstCustomIteratorImpl<DataT>::operator=(other);
      return *this;
    }

    CustomIteratorImpl& operator=(CustomIteratorImpl&& other) {
      ConstCustomIteratorImpl<DataT>::operator=(std::move(other));
      return *this;
    }
  };

  template <typename DataT>
  class TestContainer {
  public:
    DECLARE_INPUT_ITERATOR(InIterator, TestContainer<DataT>, \
			   ConstCustomIteratorImpl<DataT>);
    DECLARE_OUTPUT_ITERATOR(OutIterator, TestContainer<DataT>, \
			    CustomIteratorImpl<DataT>);
    DECLARE_FORWARD_ITERATORS(FwdIterator, TestContainer<DataT>, \
			      ConstCustomIteratorImpl<DataT>, \
			      CustomIteratorImpl<DataT>);
    DECLARE_BIDI_ITERATORS(BidiIterator, TestContainer<DataT>, \
			   ConstCustomIteratorImpl<DataT>, \
			   CustomIteratorImpl<DataT>);
    DECLARE_RANDOM_ACCESS_ITERATORS(RndIterator, TestContainer<DataT>, \
				    ConstCustomIteratorImpl<DataT>, \
				    CustomIteratorImpl<DataT>);

  public:
    TestContainer(const std::initializer_list<DataT>& data):
        _data(new DataT[data.size()]), _end(_data + data.size()) {
      std::copy(data.begin(), data.end(), _data);
    }
    TestContainer(const TestContainer<DataT>& other):
        _data(new DataT[other.size()]), _end(_data + other.size()) {
      std::copy(other._data, other._end, _data);
    }
    TestContainer(TestContainer<DataT>&& other):
        _data(other._data), _end(other._end) {
      other._data= nullptr;
      other._end= nullptr;
    }
    ~TestContainer() {
      delete[] _data;
    }

    size_t size() const { return (size_t)(_end - _data); }
    
    template <typename IterTagT>
    void begin(const IterTagT& tag) const {
      static_assert(sizeof(IterTagT) == 0, "Invalid iterator tag");
    }
    InIterator begin(const std::input_iterator_tag& tag) const {
      return InIterator(ConstCustomIteratorImpl<DataT>(_data));
    }
    OutIterator begin(const std::output_iterator_tag& tag) const {
      return OutIterator(CustomIteratorImpl<DataT>(_data));
    }
    ConstFwdIterator begin(const std::forward_iterator_tag& tag) const {
      return ConstFwdIterator(ConstCustomIteratorImpl<DataT>(_data));
    }
    FwdIterator begin(const std::forward_iterator_tag& tag) {
      return FwdIterator(CustomIteratorImpl<DataT>(_data));
    }
    ConstBidiIterator begin(const std::bidirectional_iterator_tag& tag) const {
      return ConstBidiIterator(ConstCustomIteratorImpl<DataT>(_data));
    }
    BidiIterator begin(const std::bidirectional_iterator_tag& tag) {
      return BidiIterator(CustomIteratorImpl<DataT>(_data));
    }
    ConstRndIterator begin(const std::random_access_iterator_tag& tag) const {
      return ConstRndIterator(ConstCustomIteratorImpl<DataT>(_data));
    }
    RndIterator begin(const std::random_access_iterator_tag& tag) {
      return RndIterator(CustomIteratorImpl<DataT>(_data));
    }

    template <typename IterTagT>
    void end(const IterTagT& tag) const {
      static_assert(sizeof(IterTagT) == 0, "Invalid iterator tag");
    }
    InIterator end(const std::input_iterator_tag& tag) const {
      return InIterator(ConstCustomIteratorImpl<DataT>(_end));
    }
    OutIterator end(const std::output_iterator_tag& tag) const {
      return OutIterator(CustomIteratorImpl<DataT>(_end));
    }
    ConstFwdIterator end(const std::forward_iterator_tag& tag) const {
      return ConstFwdIterator(ConstCustomIteratorImpl<DataT>(_end));
    }
    FwdIterator end(const std::forward_iterator_tag& tag) {
      return FwdIterator(CustomIteratorImpl<DataT>(_end));
    }
    ConstBidiIterator end(const std::bidirectional_iterator_tag& tag) const {
      return ConstBidiIterator(ConstCustomIteratorImpl<DataT>(_end));
    }
    BidiIterator end(const std::bidirectional_iterator_tag& tag) {
      return BidiIterator(CustomIteratorImpl<DataT>(_end));
    }
    ConstRndIterator end(const std::random_access_iterator_tag& tag) const {
      return ConstRndIterator(ConstCustomIteratorImpl<DataT>(_end));
    }
    RndIterator end(const std::random_access_iterator_tag& tag) {
      return RndIterator(CustomIteratorImpl<DataT>(_end));
    }

    TestContainer<DataT>& operator=(const TestContainer<DataT>& other) {
      delete[] _data;
      _data= new DataT[other.size()];
      _end= _data + other.size();
      std::copy(other._data, other._end, _data);
      return *this;
    }
    TestContainer<DataT>& operator=(TestContainer<DataT>&& other) {
      _data= other._data;
      _end= other._end;
      other._data= nullptr;
      other._end= nullptr;
      return *this;
    }
    const DataT& operator[](size_t i) const { return _data[i]; }
    DataT& operator[](size_t i) { return _data[i]; }

  private:
    DataT* _data;
    DataT* _end;
  };

  template <typename IterT>
  std::string join(const std::string& sep,const IterT& begin,
		   const IterT& end) {
    std::ostringstream tmp;
    for (IterT i= begin; i != end; ++i) {
      if (i != begin) {
	tmp << sep;
      }
      tmp << *i;
    }
    return tmp.str();
  }

  template <typename IterT, typename DataT>
  std::string preIncrementTest(const IterT& begin, const IterT& end,
			       const std::vector<DataT>& truth) {
    IterT i(begin);
    typename std::vector<DataT>::const_iterator j(truth.begin());
    std::ostringstream trueItemsStream;
    std::ostringstream iterItemsStream;
    int cnt= 0;

    while (i != end) {
      if (j == truth.end()) {
	std::ostringstream msg;
	msg << "Sequence [" << iterItemsStream.str()
	    << "...] has to many items; it should be [" << trueItemsStream.str()
	    << "]";
	return msg.str();
      }
      if (cnt) {
	trueItemsStream << ", ";
	iterItemsStream << ", ";
      }

      DataT v= *i;
      iterItemsStream << v;
      trueItemsStream << *j;
      if (v != *j) {
	std::ostringstream msg;
	msg << "Item #" << (cnt+1) << " in [" << iterItemsStream.str()
	    << "] is " << v << "; it should be " << *j;
	return msg.str();
      }
      ++i;
      ++j;
      ++cnt;
    }
    if (j != truth.end()) {
      std::ostringstream msg;
      while (j != truth.end()) {
	if (j != truth.begin()) {
	  trueItemsStream << ", ";
	}
	trueItemsStream << *j;
	++j;
      }
      msg << "Sequence [" << iterItemsStream.str()
	  << "] ended prematurely.  It should be " << trueItemsStream.str()
	  << "]";
      return msg.str();
    }
    return std::string();
  }

  template <typename IterT, typename DataT>
  std::string postIncrementTest(const IterT& begin, const IterT& end,
				const std::vector<DataT>& truth) {
    IterT i(begin);
    typename std::vector<DataT>::const_iterator j(truth.begin());
    std::ostringstream trueItemsStream;
    std::ostringstream iterItemsStream;
    int cnt= 0;

    while (i != end) {
      if (j == truth.end()) {
	std::ostringstream msg;
	msg << "Sequence [" << iterItemsStream.str()
	    << "...] has to many items; it should be ["
	    << trueItemsStream.str() << "]";
	return msg.str();
      }
      if (cnt) {
	trueItemsStream << ", ";
	iterItemsStream << ", ";
      }

      IterT k= i++;
      DataT v= *k;
      iterItemsStream << v;
      trueItemsStream << *j;
      if (v != *j) {
	std::ostringstream msg;
	msg << "Item #" << (cnt+1) << " in [" << iterItemsStream.str()
	    << "] is " << v << "; it should be " << *j;
	return msg.str();
      }
      ++j;
      ++cnt;
    }
    if (j != truth.end()) {
      std::ostringstream msg;
      while (j != truth.end()) {
	if (j != truth.begin()) {
	  trueItemsStream << ", ";
	}
	trueItemsStream << *j;
	++j;
      }
      msg << "Sequence [" << iterItemsStream.str()
	  << "] ended prematurely.  It should be " << trueItemsStream.str()
	  << "]";
      return msg.str();
    }
    return std::string();
  }

  template <typename IterT, typename DataT>
  std::string preDecrementTest(const IterT& begin, const IterT& end,
			       const std::vector<DataT>& truth) {
    IterT i(end);
    typename std::vector<DataT>::const_iterator j(truth.begin());
    std::ostringstream trueItemsStream;
    std::ostringstream iterItemsStream;
    int cnt= 0;

    while (i != begin) {
      if (j == truth.end()) {
	std::ostringstream msg;
	msg << "Sequence [" << iterItemsStream.str()
	    << "...] has to many items; it should be ["
	    << trueItemsStream.str() << "]";
	return msg.str();
      }
      if (cnt) {
	trueItemsStream << ", ";
	iterItemsStream << ", ";
      }

      --i;
      DataT v= *i;
      iterItemsStream << v;
      trueItemsStream << *j;
      if (v != *j) {
	std::ostringstream msg;
	msg << "Item #" << (cnt+1) << " in [" << iterItemsStream.str()
	    << "] is " << v << "; it should be " << *j;
	return msg.str();
      }
      ++j;
      ++cnt;
    }
    if (j != truth.end()) {
      std::ostringstream msg;
      while (j != truth.end()) {
	if (j != truth.begin()) {
	  trueItemsStream << ", ";
	}
	trueItemsStream << *j;
	++j;
      }
      msg << "Sequence [" << iterItemsStream.str()
	  << "] ended prematurely.  It should be " << trueItemsStream.str()
	  << "]";
      return msg.str();
    }
    return std::string();
  }

  template <typename IterT, typename DataT>
  std::string postDecrementTest(const IterT& begin, const IterT& end,
				const std::vector<DataT>& truth) {
    IterT i(end);
    typename std::vector<DataT>::const_iterator j(truth.begin());
    std::ostringstream trueItemsStream;
    std::ostringstream iterItemsStream;
    int cnt= 0;

    while (i != begin) {
      if (j == truth.end()) {
	std::ostringstream msg;
	msg << "Sequence [" << iterItemsStream.str()
	    << "...] has to many items; it should be ["
	    << trueItemsStream.str() << "]";
	return msg.str();
      }
      if (cnt) {
	trueItemsStream << ", ";
	iterItemsStream << ", ";
      }

      IterT k= i--;
      DataT v= *i;
      ++cnt;
      if (--k != i) {
	std::ostringstream msg;
	msg << "After " << cnt
	    << " decrements, the post-decrement operator failed to return the correct value.";
	return msg.str();
      }
      iterItemsStream << v;
      trueItemsStream << *j;
      if (v != *j) {
	std::ostringstream msg;
	msg << "Item #" << (cnt+1) << " in [" << iterItemsStream.str()
	    << "] is " << v << "; it should be " << *j;
	return msg.str();
      }
      ++j;
    }
    if (j != truth.end()) {
      std::ostringstream msg;
      while (j != truth.end()) {
	if (j != truth.begin()) {
	  trueItemsStream << ", ";
	}
	trueItemsStream << *j;
	++j;
      }
      msg << "Sequence [" << iterItemsStream.str()
	  << "] ended prematurely.  It should be " << trueItemsStream.str()
	  << "]";
      return msg.str();
    }
    return std::string();
  }

  template <typename IterT, typename DataT>
  std::string writeThroughIteratorTest(IterT begin, IterT end, size_t offset,
				       DataT v, std::vector<DataT> truth) {
    IterT i(begin);
    for (size_t k=0;k<offset;++k,++i) { }
    *i= v;
    truth[offset]= v;
    return preIncrementTest(begin, end, truth);
  }
}

TEST(IteratorTests, InputIterator) {
  static const std::vector<int> TRUTH{1, 2, 3, 5, 4};
  TestContainer<int> data{ 1, 2, 3, 5, 4 };
  TestContainer<int>::InIterator p(data.begin(std::input_iterator_tag()));
  TestContainer<int>::InIterator end;
  std::string errMsg;

  end= data.end(std::input_iterator_tag());
  EXPECT_TRUE(p == data.begin(std::input_iterator_tag()));
  EXPECT_TRUE(end == data.end(std::input_iterator_tag()));
  errMsg= preIncrementTest(data.begin(std::input_iterator_tag()),
			   data.end(std::input_iterator_tag()),
			   TRUTH);
  EXPECT_TRUE(errMsg.empty()) << errMsg;
  errMsg= postIncrementTest(data.begin(std::input_iterator_tag()),
			    data.end(std::input_iterator_tag()),
			    TRUTH);
  EXPECT_TRUE(errMsg.empty()) << errMsg;
}

TEST(IteratorTests, OutputIterator) {
  TestContainer<int> data{0,0,0};
  TestContainer<int>::OutIterator p(data.begin(std::output_iterator_tag()));

  *p++= 1;
  *p= 2;
  ++p;
  *p= 3;
  EXPECT_EQ(data[0], 1);
  EXPECT_EQ(data[1], 2);
  EXPECT_EQ(data[2], 3);

  p= data.begin(std::output_iterator_tag());
  *p= 10;
  ++p;
  *p= 11;
  ++p;
  *p= 12;
  EXPECT_EQ(data[0], 10);
  EXPECT_EQ(data[1], 11);
  EXPECT_EQ(data[2], 12);
}

TEST(IteratorTests, ConstForwardIterator) {
  static const std::vector<int> TRUTH{ 1, 2, 3, 5, 4 };
  TestContainer<int> data{ 1, 2, 3, 5, 4 };
  const TestContainer<int>& cdata= data;
  TestContainer<int>::ConstFwdIterator p(cdata.begin(std::forward_iterator_tag()));
  TestContainer<int>::ConstFwdIterator end;
  std::string errMsg;

  end= cdata.end(std::forward_iterator_tag());
  EXPECT_TRUE(p == cdata.begin(std::forward_iterator_tag()));
  EXPECT_TRUE(end == cdata.end(std::forward_iterator_tag()));
  errMsg= preIncrementTest(cdata.begin(std::forward_iterator_tag()),
			   cdata.end(std::forward_iterator_tag()),
			   TRUTH);
  EXPECT_TRUE(errMsg.empty()) << errMsg;
  errMsg= postIncrementTest(cdata.begin(std::forward_iterator_tag()),
			    cdata.end(std::forward_iterator_tag()),
			    TRUTH);
  EXPECT_TRUE(errMsg.empty()) << errMsg;
}

TEST(IteratorTests, ForwardIterator) {
  static const std::vector<int> TRUTH{ 1, 2, 3, 5, 4 };
  TestContainer<int> data{ 1, 2, 3, 5, 4 };
  const TestContainer<int>& cdata= data;
  TestContainer<int>::FwdIterator p(data.begin(std::forward_iterator_tag()));
  TestContainer<int>::FwdIterator end;
  std::string errMsg;

  end= data.end(std::forward_iterator_tag());
  EXPECT_TRUE(p == data.begin(std::forward_iterator_tag()));
  EXPECT_TRUE(end == data.end(std::forward_iterator_tag()));
  errMsg= preIncrementTest(data.begin(std::forward_iterator_tag()),
			   data.end(std::forward_iterator_tag()),
			   TRUTH);
  EXPECT_TRUE(errMsg.empty()) << errMsg;
  errMsg= postIncrementTest(data.begin(std::forward_iterator_tag()),
			    data.end(std::forward_iterator_tag()),
			    TRUTH);
  EXPECT_TRUE(errMsg.empty()) << errMsg;
  errMsg= writeThroughIteratorTest(data.begin(std::forward_iterator_tag()),
				   data.end(std::forward_iterator_tag()),
				   2, 100, TRUTH);

  // Test ability to construct a const iterator from a mutable iterator,
  // assign a mutable iterator to a const iterator and compare const
  // and mutable iterators
  TestContainer<int>::ConstFwdIterator ci(p);
  EXPECT_TRUE(ci == cdata.begin(std::forward_iterator_tag()));
  EXPECT_TRUE(ci == p);
  EXPECT_TRUE(ci != data.end(std::forward_iterator_tag()));
  
  ci= end;
  EXPECT_TRUE(ci == cdata.end(std::forward_iterator_tag()));
}

TEST(IteratorTests, ConstBidiIterator) {
  static const std::vector<int> TRUTH{ 1, 2, 3, 5, 4 };
  static const std::vector<int> BACKWARDS{ 4, 5, 3, 2, 1 };
  TestContainer<int> data{ 1, 2, 3, 5, 4 };
  const TestContainer<int>& cdata= data;
  TestContainer<int>::ConstBidiIterator p(cdata.begin(std::bidirectional_iterator_tag()));
  TestContainer<int>::ConstBidiIterator end;
  std::string errMsg;

  end= cdata.end(std::bidirectional_iterator_tag());
  EXPECT_TRUE(p == cdata.begin(std::bidirectional_iterator_tag()));
  EXPECT_TRUE(end == cdata.end(std::bidirectional_iterator_tag()));
  errMsg= preIncrementTest(cdata.begin(std::bidirectional_iterator_tag()),
			   cdata.end(std::bidirectional_iterator_tag()),
			   TRUTH);
  EXPECT_TRUE(errMsg.empty()) << errMsg;
  errMsg= postIncrementTest(cdata.begin(std::bidirectional_iterator_tag()),
			    cdata.end(std::bidirectional_iterator_tag()),
			    TRUTH);
  EXPECT_TRUE(errMsg.empty()) << errMsg;
  errMsg= preDecrementTest(cdata.begin(std::bidirectional_iterator_tag()),
			   cdata.end(std::bidirectional_iterator_tag()),
			   BACKWARDS);
  EXPECT_TRUE(errMsg.empty()) << errMsg;
  errMsg= postDecrementTest(cdata.begin(std::bidirectional_iterator_tag()),
			    cdata.end(std::bidirectional_iterator_tag()),
			    BACKWARDS);
  EXPECT_TRUE(errMsg.empty()) << errMsg;

}

TEST(IteratorTests, BidiIterator) {
  static const std::vector<int> TRUTH{ 1, 2, 3, 5, 4 };
  static const std::vector<int> BACKWARDS{ 4, 5, 3, 2, 1 };
  TestContainer<int> data{ 1, 2, 3, 5, 4 };
  const TestContainer<int>& cdata= data;
  TestContainer<int>::BidiIterator p(data.begin(std::bidirectional_iterator_tag()));
  TestContainer<int>::BidiIterator end;
  std::string errMsg;

  end= data.end(std::bidirectional_iterator_tag());
  EXPECT_TRUE(p == data.begin(std::bidirectional_iterator_tag()));
  EXPECT_TRUE(end == data.end(std::bidirectional_iterator_tag()));
  errMsg= preIncrementTest(data.begin(std::bidirectional_iterator_tag()),
			   data.end(std::bidirectional_iterator_tag()),
			   TRUTH);
  EXPECT_TRUE(errMsg.empty()) << errMsg;
  errMsg= postIncrementTest(data.begin(std::bidirectional_iterator_tag()),
			    data.end(std::bidirectional_iterator_tag()),
			    TRUTH);
  EXPECT_TRUE(errMsg.empty()) << errMsg;
  errMsg= preDecrementTest(data.begin(std::bidirectional_iterator_tag()),
			   data.end(std::bidirectional_iterator_tag()),
			   BACKWARDS);
  EXPECT_TRUE(errMsg.empty()) << errMsg;
  errMsg= postDecrementTest(data.begin(std::bidirectional_iterator_tag()),
			    data.end(std::bidirectional_iterator_tag()),
			    BACKWARDS);
  EXPECT_TRUE(errMsg.empty()) << errMsg;

  // Test ability to construct a const iterator from a mutable iterator,
  // assign a mutable iterator to a const iterator and compare const
  // and mutable iterators
  TestContainer<int>::ConstBidiIterator ci(p);
  EXPECT_TRUE(ci == cdata.begin(std::bidirectional_iterator_tag()));
  EXPECT_TRUE(ci == p);
  EXPECT_TRUE(ci != data.end(std::bidirectional_iterator_tag()));
  
  ci= end;
  EXPECT_TRUE(ci == cdata.end(std::bidirectional_iterator_tag()));
}

TEST(IteratorTests, ConstRandomIterator) {
  static const std::vector<int> TRUTH{ 1, 2, 3, 5, 4 };
  static const std::vector<int> BACKWARDS{ 4, 5, 3, 2, 1 };
  TestContainer<int> data{ 1, 2, 3, 5, 4 };
  const TestContainer<int>& cdata= data;
  TestContainer<int>::ConstRndIterator p(cdata.begin(std::random_access_iterator_tag()));
  TestContainer<int>::ConstRndIterator end;
  std::string errMsg;

  end= cdata.end(std::random_access_iterator_tag());
  EXPECT_TRUE(p == cdata.begin(std::random_access_iterator_tag()));
  EXPECT_TRUE(end == cdata.end(std::random_access_iterator_tag()));
  errMsg= preIncrementTest(cdata.begin(std::random_access_iterator_tag()),
			   cdata.end(std::random_access_iterator_tag()),
			   TRUTH);
  EXPECT_TRUE(errMsg.empty()) << errMsg;
  errMsg= postIncrementTest(cdata.begin(std::random_access_iterator_tag()),
			    cdata.end(std::random_access_iterator_tag()),
			    TRUTH);
  EXPECT_TRUE(errMsg.empty()) << errMsg;
  errMsg= preDecrementTest(cdata.begin(std::random_access_iterator_tag()),
			   cdata.end(std::random_access_iterator_tag()),
			   BACKWARDS);
  EXPECT_TRUE(errMsg.empty()) << errMsg;
  errMsg= postDecrementTest(cdata.begin(std::random_access_iterator_tag()),
			    cdata.end(std::random_access_iterator_tag()),
			    BACKWARDS);
  EXPECT_TRUE(errMsg.empty()) << errMsg;

  TestContainer<int>::ConstRndIterator q(p);
  EXPECT_EQ(p[4], 4);
  EXPECT_EQ(*(p+2), 3);
  EXPECT_EQ(*(end-2), 5);
  q += 3;
  EXPECT_EQ(*q, 5);
  q -= 2;
  EXPECT_EQ(*q, 2);
  EXPECT_EQ(q-p, 1);
  EXPECT_EQ(q-end, -4);
  EXPECT_TRUE(p < q);
  EXPECT_TRUE(p <= q);
  EXPECT_TRUE(p <= p);
  EXPECT_TRUE(end > q);
  EXPECT_TRUE(end >= q);
  EXPECT_TRUE(q >= q);
}

TEST(IteratorTests, RandomIterator) {
  static const std::vector<int> TRUTH{ 1, 2, 3, 5, 4 };
  static const std::vector<int> BACKWARDS{ 4, 5, 3, 2, 1 };
  TestContainer<int> data{ 1, 2, 3, 5, 4 };
  const TestContainer<int>& cdata= data;
  TestContainer<int>::RndIterator p(data.begin(std::random_access_iterator_tag()));
  TestContainer<int>::RndIterator end;
  std::string errMsg;

  end= data.end(std::random_access_iterator_tag());
  EXPECT_TRUE(p == data.begin(std::random_access_iterator_tag()));
  EXPECT_TRUE(end == data.end(std::random_access_iterator_tag()));
  errMsg= preIncrementTest(data.begin(std::random_access_iterator_tag()),
			   data.end(std::random_access_iterator_tag()),
			   TRUTH);
  EXPECT_TRUE(errMsg.empty()) << errMsg;
  errMsg= postIncrementTest(data.begin(std::random_access_iterator_tag()),
			    data.end(std::random_access_iterator_tag()),
			    TRUTH);
  EXPECT_TRUE(errMsg.empty()) << errMsg;
  errMsg= preDecrementTest(data.begin(std::random_access_iterator_tag()),
			   data.end(std::random_access_iterator_tag()),
			   BACKWARDS);
  EXPECT_TRUE(errMsg.empty()) <<  errMsg;
  errMsg= postDecrementTest(data.begin(std::random_access_iterator_tag()),
			    data.end(std::random_access_iterator_tag()),
			    BACKWARDS);
  EXPECT_TRUE(errMsg.empty()) << errMsg;

  TestContainer<int>::RndIterator q(p);
  EXPECT_EQ(p[4], 4);
  EXPECT_EQ(*(p+2), 3);
  EXPECT_EQ(*(end-2), 5);
  q += 3;
  EXPECT_EQ(*q, 5);
  q -= 2;
  EXPECT_EQ(*q, 2);
  EXPECT_EQ(q-p, 1);
  EXPECT_EQ(q-end, -4);
  EXPECT_TRUE(p < q);
  EXPECT_TRUE(p <= q);
  EXPECT_TRUE(p <= p);
  EXPECT_TRUE(end > q);
  EXPECT_TRUE(end >= q);
  EXPECT_TRUE(q >= q);

  // Test ability to construct a const iterator from a mutable iterator,
  // assign a mutable iterator to a const iterator and compare const
  // and mutable iterators
  TestContainer<int>::ConstRndIterator ci(p);
  EXPECT_TRUE(ci == cdata.begin(std::random_access_iterator_tag()));
  EXPECT_TRUE(ci == p);
  //EXPECT_TRUE(p == ci);
  EXPECT_TRUE(ci != data.end(std::random_access_iterator_tag()));
  //EXPECT_TRUE(data.end(std::random_access_iterator_tag()) != ci);
  EXPECT_TRUE(ci < end);
  EXPECT_TRUE(ci <= end);
  EXPECT_TRUE((ci + 1) > p);
  EXPECT_TRUE((ci + 1) >= p);
  //EXPECT_TRUE(p < (ci+1));
  //EXPECT_TRUE(p <= ci);
  //EXPECT_TRUE((p+1) > ci);
  //EXPECT_TRUE(p >= ci);
  
  ci= end;
  EXPECT_TRUE(ci == cdata.end(std::random_access_iterator_tag()));
}
