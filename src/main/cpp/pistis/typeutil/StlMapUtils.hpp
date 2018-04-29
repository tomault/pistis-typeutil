#ifndef __PISTIS__TYPEUTIL__STLMAPUTILS_HPP__
#define __PISTIS__TYPEUTIL__STLMAPUTILS_HPP__

#include <pistis/exceptions/NoSuchItem.hpp>
#include <functional>
#include <iterator>
#include <vector>

namespace pistis {
  namespace typeutil {
    namespace stl_map_utils {

      template <typename Map>
      class KeyIterator {
      public:
	typedef std::forward_iterator_tag iterator_category;
	typedef typename Map::key_type value_type;
	typedef const typename Map::key_type& reference;
	typedef const typename Map::key_type* pointer;
	typedef std::ptrdiff_t difference_type;

      public:
	KeyIterator() : p_() { }
	KeyIterator(const typename Map::const_iterator& p) : p_(p) { }

	const auto& operator*() const { return p_->first; }
	auto operator->() const { return &(p_->first); }
	KeyIterator& operator++() { ++p_; return *this; }
	KeyIterator  operator++(int) {
	  KeyIterator tmp(*this);
	  ++p_;
	  return tmp;
	}
	bool operator==(const KeyIterator& other) const {
	  return p_ == other.p_;
	}
	bool operator!=(const KeyIterator& other) const {
	  return p_ != other.p_;
	}

      private:
	typename Map::const_iterator p_;
      };

      template <typename Map>
      class ConstValueIterator {
      public:
	ConstValueIterator() : p_() { }
	ConstValueIterator(const typename Map::const_iterator& p) : p_(p) { }

	const auto& operator*() const { return p_->second; }
	auto operator->() const { return &(p_->second); }
	ConstValueIterator& operator++() { ++p_; return *this; }
	ConstValueIterator  operator++(int) {
	  ConstValueIterator tmp(*this);
	  ++p_;
	  return tmp;
	}
	bool operator==(const ConstValueIterator& other) const {
	  return p_ == other.p_;
	}
	bool operator!=(const ConstValueIterator& other) const {
	  return p_ != other.p_;
	}

      private:
	typename Map::const_iterator p_;
      };
      
      template <typename Map>
      class ValueIterator {
	ValueIterator() : p_() { }
	ValueIterator(const typename Map::iterator& p) : p_(p) { }

	auto& operator*() const { return p_->second; }
	auto operator->() const { return &(p_->second); }
	ValueIterator& operator++() { ++p_; return *this; }
	ValueIterator  operator++(int) {
	  ValueIterator tmp(*this);
	  ++p_;
	  return tmp;
	}
	bool operator==(const ValueIterator& other) const {
	  return p_ == other.p_;
	}
	bool operator!=(const ValueIterator& other) const {
	  return p_ != other.p_;
	}

      private:
	typename Map::iterator p_;
      };

      template <typename Map>
      KeyIterator<Map> startOfKeys(const Map& m) {
	return KeyIterator<Map>(m.begin());
      }

      template <typename Map>
      KeyIterator<Map> endOfKeys(const Map& m) {
	return KeyIterator<Map>(m.end());
      }

      template <typename Map>
      ConstValueIterator<Map> startOfValues(const Map& m) {
	return ConstValueIterator<Map>(m.begin());
      }

      template <typename Map>
      ValueIterator<Map> startOfValues(Map& m) {
	return ValueIterator<Map>(m.begin());
      }

      template <typename Map>
      ConstValueIterator<Map> endOfValues(const Map& m) {
	return ConstValueIterator<Map>(m.end());
      }

      template <typename Map>
      ValueIterator<Map> endOfValues(Map& m) {
	return ValueIterator<Map>(m.end());
      }

      template <typename Map>
      std::vector<typename Map::key_type> keys(const Map& m) {
	return std::vector<typename Map::key_type>(startOfKeys(m),
						   endOfKeys(m));
      }

      template <typename Map>
      std::vector<typename Map::mapped_type> values(const Map& m) {
	return std::vector<typename Map::mapped_type>(startOfValues(m),
						      endOfValues(m));
      }
      
      template <typename Map>
      const typename Map::mapped_type& get(
	  const Map& m, const typename Map::key_type& k
      ) {
	auto i = map.find(k);
	if (i != map.end()) {
	  return i->second;
	} else {
	  throw pistis::exceptions::NoSuchItem("", PISTIS_EX_HERE);
	}
      }

      template <typename Map>
      const typename Map::mapped_type& get(
	  const Map& m, const typename Map::key_type& k,
	  std::function<std::string (const typename Map::key_type&)> name
      ) {
	auto i = map.find(k);
	if (i == map.end()) {
	  throw pistis::exceptions::NoSuchItem(name(), PISTIS_EX_HERE);
	}
	return i->second;
      }

      template <typename Map>
      typename Map::mapped_type& get(Map& m,
				     const typename Map::key_type& k) {
	auto i = map.find(k);
	if (i == map.end()) {
	  throw pistis::exceptions::NoSuchItem("", PISTIS_EX_HERE);
	}
	return i->second;
      }

      template <typename Map>
      typename Map::mapped_type& get(
	  Map& m, const typename Map::key_type& k,
	  std::function< std::string (const typename Map::key_type&) > name
      ) {
	auto i = map.find(k);
	if (i == map.end()) {
	  throw pistis::exceptions::NoSuchItem(name(), PISTIS_EX_HERE);
	}
	return i->second;
      }

      template <typename Map>
      const typename Map::mapped_type& get(
	  const Map& m, const typename Map::key_type& k,
	  const typename Map::mapped_type& dv
      ) {
	auto i = map.find(k);
	return (i != map.end()) ? i->second : dv;
      }

      template <typename Map>
      Map::mapped_type& get(Map& m, const typename Map::key_type& k,
			    Map::mapped_type& dv) {
	auto i = map.find(k);
	return (i != map.end()) ? i->second : dv;
      }

      template <typename Map, typename Function>
      typename Map::mapped_type getOrCall(const Map& m,
					  const typename Map::key_type& k,
					  Function f) {
	auto i = map.find(k);
	return (i != map.end()) ? i->second : f();
      }

      template <typename Map, typename Function>
      typename Map::mapped_type getOrUpdate(Map& m,
					    const typename Map::mapped_type& k,
					    Function f) {
	auto i = map.find(k);
	if (i != map.end()) {
	  return i->second;
	} else {
	  auto j = m.insert(std::make_pair(k, f()));
	  return j.first->second;
	}
      }

      
    }
  }
}
#endif
