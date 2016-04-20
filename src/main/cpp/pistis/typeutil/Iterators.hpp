#ifndef __PISTIS__TYPEUTIL__ITERATORS_HPP__
#define __PISTIS__TYPEUTIL__ITERATORS_HPP__

#include <pistis/typeutil/HasMember.hpp>
#include <iterator>
#include <type_traits>
#include <stddef.h>

namespace pistis {
  namespace typeutil {
    namespace detail {
      DECLARE_HAS_MEMBER_TYPE(HasValueType, ValueType);
      DECLARE_HAS_MEMBER_TYPE(HasDistanceType, DistanceType);
      DECLARE_HAS_MEMBER_TYPE(HasSTLValueType, value_type);
      DECLARE_HAS_MEMBER_TYPE(HasSTLDifferenceType, difference_type);

      template <typename T> struct Identity { typedef T type; };
      
      template <typename T>
      struct ExtractPointerType { typedef typename T::PointerType type; };

      template <typename T>
      struct ExtractValueType { typedef typename T::ValueType type; };

      template <typename T>
      struct ExtractDistanceType { typedef typename T::DistanceType type; };

      template <typename T>
      struct ExtractSTLValueType { typedef typename T::value_type type; };

      template <typename T>
      struct ExtractSTLDifferenceType {
	typedef typename T::difference_type type;
      };
      
      template <typename T, typename ReferenceT>
      struct DeduceValueType {
	typedef typename std::conditional<
	    HasValueType<T>::value,
	    ExtractValueType<T>,
	    typename std::conditional<
	        HasSTLValueType<T>::value,
                ExtractSTLValueType<T>,
                typename std::remove_cv<
		    typename std::remove_reference<ReferenceT>::type
		>
	    >::type
	>::type::type type;
      };

      template <typename T>
      struct DeduceDistanceType {
	typedef typename std::conditional<
	    HasDistanceType<T>::value,
            ExtractDistanceType<T>,
	    typename std::conditional<
	        HasSTLDifferenceType<T>::value,
                ExtractSTLDifferenceType<T>,
                Identity<ptrdiff_t>
	    >::type
	>::type::type type;
      };
    }

    template <typename ImplT>
    struct IteratorImplTraits {
      typedef decltype(((ImplT*)0)->operator*()) ReferenceType;
      typedef decltype(((ImplT*)0)->operator->()) PointerType;
      typedef typename detail::DeduceValueType<ImplT, ReferenceType>::type
              ValueType;
      typedef typename detail::DeduceDistanceType<ImplT>::type DistanceType;
    };

    template <typename PtrT>
    struct IteratorImplTraits<PtrT*> {
      typedef PtrT& ReferenceType;
      typedef PtrT* PointerType;
      typedef PtrT  ValuueType;
      typedef ptrdiff_t DistanceType;
    };

    template <typename PtrT>
    struct IteratorImplTraits<const PtrT*> {
      typedef const PtrT& ReferenceType;
      typedef const PtrT* PointerType;
      typedef const PtrT  ValueType;
      typedef ptrdiff_t DistanceType;
    };

    namespace detail {
      template <typename DerivedT, typename ImplT>
      struct BasicIteratorOps {
	typename IteratorImplTraits<ImplT>::ReferenceType operator*() const {
	  return *static_cast<const DerivedT&>(*this)._p;
	}
	DerivedT& operator++() {
	  ++static_cast<DerivedT&>(*this)._p;
	  return static_cast<DerivedT&>(*this);
	}
	DerivedT  operator++(int) {
	  DerivedT tmp(static_cast<DerivedT&>(*this));
	  ++static_cast<DerivedT&>(*this)._p;
	  return tmp;
	}
      };

      template <typename DerivedT, typename ImplT>
      struct IteratorPointerOp {
	typename IteratorImplTraits<ImplT>::PointerType operator->() const {
	  return static_cast<DerivedT&>(*this).operator->();
	}
      };

      template <typename DerivedT, typename ImplT>
      struct IteratorEqualityOps {
	bool operator==(const DerivedT& other) const {
	  return static_cast<const DerivedT&>(*this)._p == other._p;
	}
	bool operator!=(const DerivedT& other) const {
	  return static_cast<const DerivedT&>(*this)._p != other._p;
	}

      protected:
	bool operator==(const ImplT& p) const {
	  return static_cast<const DerivedT&>(*this)._p == p;
	}
	bool operator!=(const ImplT& p) const {
	  return static_cast<const DerivedT&>(*this)._p != p;
	}
      };

      template <typename DerivedT, typename ImplT>
      struct IteratorBackwardOps {
	DerivedT& operator--() {
	  --static_cast<DerivedT&>(*this)._p;
	  return static_cast<DerivedT&>(*this);
	}
	DerivedT  operator--(int) {
	  DerivedT tmp(static_cast<DerivedT&>(*this));
	  --static_cast<DerivedT&>(*this)._p;
	  return tmp;
	}
      };

      template <typename DerivedT, typename ImplT>
      struct IteratorRandomAccessOps {
	DerivedT operator+(
	    typename IteratorImplTraits<ImplT>::DistanceType n
	) const {
	  return DerivedT(static_cast<const DerivedT&>(*this)._p + n);
	}

	DerivedT operator-(
	    typename IteratorImplTraits<ImplT>::DistanceType n
	) const {
	  return DerivedT(static_cast<const DerivedT&>(*this)._p - n);
	}
	
	DerivedT& operator+=(
	    typename IteratorImplTraits<ImplT>::DistanceType n
	) {
	  static_cast<DerivedT&>(*this)._p += n;
	  return static_cast<DerivedT&>(*this);
	}
	
	DerivedT& operator-=(
	    typename IteratorImplTraits<ImplT>::DistanceType n
	) {
	  static_cast<DerivedT&>(*this)._p -= n;
	  return static_cast<DerivedT&>(*this);
	}

	typename IteratorImplTraits<ImplT>::ReferenceType operator[](
	    typename IteratorImplTraits<ImplT>::DistanceType n
	) const {
	  return static_cast<const DerivedT&>(*this)._p[n];
	}
	
	typename IteratorImplTraits<ImplT>::DistanceType operator-(
	    const DerivedT& other
	) const {
	  return static_cast<const DerivedT&>(*this)._p - other._p;
	}
	
	bool operator<(const DerivedT& other) const {
	  return static_cast<const DerivedT&>(*this)._p < other._p;
	}
	
	bool operator>(const DerivedT& other) const {
	  return static_cast<const DerivedT&>(*this)._p > other._p;
	}
	
	bool operator<=(const DerivedT& other) const {
	  return static_cast<const DerivedT&>(*this)._p <= other._p;
	}
	
	bool operator>=(const DerivedT& other) const {
	  return static_cast<const DerivedT&>(*this)._p >= other._p;
	}
      };

      template <typename DerivedT, typename ImplT>
      class AnyIterator {
      public:
	typedef typename IteratorImplTraits<ImplT>::ReferenceType ReferenceType;
	typedef typename IteratorImplTraits<ImplT>::PointerType PointerType;
	typedef typename IteratorImplTraits<ImplT>::ValueType ValueType;
	typedef typename IteratorImplTraits<ImplT>::DistanceType DistanceType;

	typedef ReferenceType reference;
	typedef PointerType pointer;
	typedef ValueType value_type;
	typedef DistanceType difference_type;

      public:
	DerivedT& operator=(const AnyIterator<DerivedT, ImplT>& other) {
	  _p= other._p;
	  return static_cast<DerivedT&>(*this);
	}
	DerivedT& operator=(AnyIterator<DerivedT, ImplT>&& other) {
	  _p= std::move(other._p);
	  return static_cast<DerivedT&>(*this);
	}

      protected:
	AnyIterator(): _p() { }
	AnyIterator(const ImplT& p): _p(p) { }
	AnyIterator(ImplT&& p): _p(std::move(p)) { }
	AnyIterator(const AnyIterator<DerivedT, ImplT>& other):
	  _p(other._p) {
	}
	AnyIterator(AnyIterator<DerivedT, ImplT>&& other):
	  _p(std::move(other._p)) {
	}

	const ImplT& _ptr() const { return this->_p; }
	ImplT& _ptr() { return this->_p; }
	ImplT&& _moveablePtr() { return std::move(this->_p); }
	void _setPtr(const ImplT& p) { _p= p; }
	void _setPtr(ImplT&& p) { _p= std::move(p); }

      private:
	ImplT _p;

	friend class BasicIteratorOps<DerivedT, ImplT>;
	friend class IteratorPointerOp<DerivedT, ImplT>;
	friend class IteratorEqualityOps<DerivedT, ImplT>;
	friend class IteratorBackwardOps<DerivedT, ImplT>;
	friend class IteratorRandomAccessOps<DerivedT, ImplT>;
      };
    }

    template <typename DerivedT, typename ImplT>
    class InputIterator :
        public detail::AnyIterator<DerivedT, ImplT>,
        public detail::BasicIteratorOps<DerivedT, ImplT>,
        public detail::IteratorEqualityOps<DerivedT, ImplT>,
        public detail::IteratorPointerOp<DerivedT, ImplT> {
    public:
      typedef std::input_iterator_tag IteratorCategoryType;
      typedef IteratorCategoryType iterator_category;

    protected:
      InputIterator() { }
      InputIterator(const ImplT& p):
	  detail::AnyIterator<DerivedT, ImplT>(p) {
	// Intentionally left blank
      }
      
      InputIterator(ImplT&& p):
          detail::AnyIterator<DerivedT, ImplT>(std::move(p)) {
	// Intentionally left blank
      }

      InputIterator(const InputIterator<DerivedT, ImplT>& other):
	  detail::AnyIterator<DerivedT, ImplT>(other) {
	// Intentionally left blank
      }
      
      InputIterator(InputIterator&& other):
	  detail::AnyIterator<DerivedT, ImplT>(std::move(other)) {
	// Intentionally left blank
      }

      InputIterator& operator=(const InputIterator&)= default;
      InputIterator& operator=(InputIterator&&)= default;
    };

    template <typename DerivedT, typename ImplT>
    class OutputIterator :
        public detail::AnyIterator<DerivedT, ImplT>,
	public detail::BasicIteratorOps<DerivedT, ImplT> {
    public:
	typedef std::output_iterator_tag IteratorCategoryType;
	typedef IteratorCategoryType iterator_category;

    protected:
      OutputIterator() { }
      OutputIterator(const ImplT& p):
	  detail::AnyIterator<DerivedT, ImplT>(p) {
	// Intentionally left blank
      }
      
      OutputIterator(ImplT&& p):
	  detail::AnyIterator<DerivedT, ImplT>(std::move(p)) {
	// Intentionally left blank
      }
      
      OutputIterator(const OutputIterator<DerivedT, ImplT>& other):
	  detail::AnyIterator<DerivedT, ImplT>(other) {
	// Intentionally left blank
      }
      
      OutputIterator(OutputIterator&& other):
	  detail::AnyIterator<DerivedT, ImplT>(std::move(other)) {
	// Intentionally left blank
      }

      OutputIterator& operator=(const OutputIterator&)= default;
      OutputIterator& operator=(OutputIterator&&)= default;
    };

    template <typename DerivedT, typename ImplT>
    class ForwardIterator :
	public detail::AnyIterator<DerivedT, ImplT>,
	public detail::BasicIteratorOps<DerivedT, ImplT>,
	public detail::IteratorEqualityOps<DerivedT, ImplT>,
	public detail::IteratorPointerOp<DerivedT, ImplT> {
    public:
      typedef std::forward_iterator_tag IteratorCategoryType;
      typedef IteratorCategoryType iterator_category;

    protected:
      ForwardIterator() { }

      ForwardIterator(const ImplT& p):
	  detail::AnyIterator<DerivedT, ImplT>(p) {
	// Intentionally left blank
      }

      ForwardIterator(ImplT&& p):
	  detail::AnyIterator<DerivedT, ImplT>(std::move(p)) {
	// Intentionally left blank
      }

      ForwardIterator(const ForwardIterator<DerivedT, ImplT>& other):
	  detail::AnyIterator<DerivedT, ImplT>(other) {
	// Intentionally left blank
      }

      ForwardIterator(ForwardIterator&& other):
	  detail::AnyIterator<DerivedT, ImplT>(std::move(other)) {
	// Intentionally left blank
      }      

      ForwardIterator& operator=(const ForwardIterator&)= default;
      ForwardIterator& operator=(ForwardIterator&&)= default;
    };

    template <typename DerivedT, typename ImplT>
    class BidirectionalIterator :
	public detail::AnyIterator<DerivedT, ImplT>,
	public detail::BasicIteratorOps<DerivedT, ImplT>,
	public detail::IteratorBackwardOps<DerivedT, ImplT>,
	public detail::IteratorEqualityOps<DerivedT, ImplT>,
	public detail::IteratorPointerOp<DerivedT, ImplT> {
    public:
      typedef std::bidirectional_iterator_tag IteratorCategoryType;
      typedef IteratorCategoryType iterator_category;

    protected:
      BidirectionalIterator() { }
      BidirectionalIterator(const ImplT& p):
	  detail::AnyIterator<DerivedT, ImplT>(p) {
	// Intentionally left blank
      }
      BidirectionalIterator(ImplT&& p):
	  detail::AnyIterator<DerivedT, ImplT>(std::move(p)) {
	// Intentionally left blank
      }
      BidirectionalIterator(
          const BidirectionalIterator<DerivedT, ImplT>& other
      ):
	  detail::AnyIterator<DerivedT, ImplT>(other) {
	// Intentionally left blank
      }
      BidirectionalIterator(BidirectionalIterator&& other):
	  detail::AnyIterator<DerivedT, ImplT>(std::move(other)) {
	// Intentionally left blank
      }      

      BidirectionalIterator& operator=(const BidirectionalIterator&)= default;
      BidirectionalIterator& operator=(BidirectionalIterator&&)= default;
    };

    template <typename DerivedT, typename ImplT>
    class RandomAccessIterator :
	public detail::AnyIterator<DerivedT, ImplT>,
	public detail::BasicIteratorOps<DerivedT, ImplT>,
	public detail::IteratorBackwardOps<DerivedT, ImplT>,
	public detail::IteratorEqualityOps<DerivedT, ImplT>,
	public detail::IteratorPointerOp<DerivedT, ImplT>,
	public detail::IteratorRandomAccessOps<DerivedT, ImplT> {
    public:
      typedef std::random_access_iterator_tag IteratorCategoryType;
      typedef IteratorCategoryType iterator_category;

    protected:
      RandomAccessIterator() { }
      RandomAccessIterator(const ImplT& p):
	  detail::AnyIterator<DerivedT, ImplT>(p) {
	// Intentionally left blank
      }
      RandomAccessIterator(ImplT&& p):
	  detail::AnyIterator<DerivedT, ImplT>(std::move(p)) {
	// Intentionally left blank
      }
      RandomAccessIterator(const RandomAccessIterator<DerivedT, ImplT>& other):
	  detail::AnyIterator<DerivedT, ImplT>(other) {
	// Intentionally left blank
      }
      RandomAccessIterator(RandomAccessIterator&& other):
	  detail::AnyIterator<DerivedT, ImplT>(std::move(other)) {
	// Intentionally left blank
      }      

      RandomAccessIterator& operator=(const RandomAccessIterator&)= default;
      RandomAccessIterator& operator=(RandomAccessIterator&&)= default;
    };

    template <typename DerivedT>
    struct DefaultPointerOp {
      typename IteratorImplTraits<DerivedT>::ReferenceType* operator->() const {
	return &(*static_cast<const DerivedT&>(*this));
      }
    };

    template <typename DerivedT>
    struct DefaultInequalityOp {
      bool operator!=(const DerivedT& other) const {
	return !(*this == other);
      }
    };

    template <typename DerivedT>
    struct DefaultGreaterThanOps {
      bool operator>(const DerivedT& other) const {
	return !(*this <= other);
      }
      bool operator>=(const DerivedT& other) const {
	return !(*this < other);
      }
    };

    template <typename DerivedT>
    struct DefaultLessEqualOp {
      bool operator<=(const DerivedT& other) const {
	return (*this < other) || (*this == other);
      }
    };

    template <typename DerivedT>
    struct DefaultRandomMoveOps {
      DerivedT operator+(
          typename IteratorImplTraits<DerivedT>::DistanceType n
      ) const {
	DerivedT tmp(*this);
	tmp += n;
	return tmp;
      }

      DerivedT operator-(
          typename IteratorImplTraits<DerivedT>::DistanceType n
      ) const {
	DerivedT tmp(*this);
	tmp -= n;
	return tmp;
      }
    };

    template <typename DerivedT>
    struct DefaultInPlaceRandomMoveOps {
      DerivedT& operator+=(
          typename IteratorImplTraits<DerivedT>::DistanceType n
      ) {
	static_cast<DerivedT&>(*this)._setPtr(
	    static_cast<DerivedT&>(*this)._ptr() + n
	);
	return *this;
      }

      DerivedT& operator-=(
          typename IteratorImplTraits<DerivedT>::DistanceType n
      ) {
	  static_cast<DerivedT&>(*this)._setPtr(
	      static_cast<DerivedT&>(*this)._ptr() - n
	  );
	  return *this;
      }
    };

  }
}

#define DECLARE_AN_ITERATOR(NAME, CONTAINER, IMPL, BASE_CLASS)               \
  class NAME : public BASE_CLASS<NAME, IMPL> {                               \
  public:                                                                    \
    NAME(): BASE_CLASS<NAME, IMPL>() { }				     \
    NAME(const NAME& other): BASE_CLASS<NAME, IMPL>(other) { }               \
    NAME(NAME&& other): BASE_CLASS<NAME, IMPL>(std::move(other)) { }         \
                                                                             \
    NAME& operator=(const NAME& other) {                                     \
      BASE_CLASS<NAME, IMPL>::operator=(other);                              \
      return *this;                                                          \
    }                                                                        \
  private:                                                                   \
    NAME(const IMPL& p): BASE_CLASS<NAME, IMPL>(p) { }                       \
    NAME(IMPL&& p): BASE_CLASS<NAME, IMPL>(std::move(p)) { }                 \
    friend class CONTAINER;                                                  \
  };

#define DECLARE_ITERATOR_PAIR(NAME, CONTAINER, C_IMPL, M_IMPL, BASE_CLASS)   \
  class Const##NAME;                                                         \
  class NAME : public BASE_CLASS<NAME, M_IMPL> {                             \
  public:                                                                    \
    NAME(): BASE_CLASS<NAME, M_IMPL>() { }                                   \
    NAME(const NAME& other): BASE_CLASS<NAME, M_IMPL>(other) { }             \
    NAME(NAME&& other): BASE_CLASS<NAME, M_IMPL>(std::move(other)) { }       \
  						                             \
    NAME& operator=(const NAME& other) {                                     \
      BASE_CLASS<NAME, M_IMPL>::operator=(other);                            \
      return *this;                                                          \
    }                                                                        \
  protected:                                                                 \
    using BASE_CLASS<NAME, M_IMPL>::_ptr;                                    \
    using BASE_CLASS<NAME, M_IMPL>::_moveablePtr;                            \
                                                                             \
  private:                                                                   \
    NAME(const M_IMPL& other): BASE_CLASS<NAME, M_IMPL>(other) { }           \
    NAME(M_IMPL&& other): BASE_CLASS<NAME, M_IMPL>(std::move(other)) { }     \
    friend class CONTAINER;                                                  \
    friend class Const##NAME;                                                \
    friend class pistis::typeutil::detail::IteratorRandomAccessOps<NAME, M_IMPL>; \
  };                                                                         \
  class Const##NAME : public BASE_CLASS<Const##NAME, C_IMPL> {               \
  public:                                                                    \
    Const##NAME(): BASE_CLASS<Const##NAME, C_IMPL>() { }                     \
    Const##NAME(const Const##NAME& other):                                   \
      BASE_CLASS<Const##NAME, C_IMPL>(other) {                               \
    }                                                                        \
    Const##NAME(Const##NAME&& other):                                        \
      BASE_CLASS<Const##NAME, C_IMPL>(std::move(other)) {                    \
    }                                                                        \
    Const##NAME(const NAME& other):                                          \
      BASE_CLASS<Const##NAME, C_IMPL>(other._ptr()) {                        \
    }                                                                        \
    Const##NAME(NAME&& other):                                               \
      BASE_CLASS<Const##NAME, C_IMPL>(other._moveablePtr()) {                \
    }                                                                        \
                                                                             \
    using BASE_CLASS<Const##NAME, C_IMPL>::operator=;                        \
    Const##NAME& operator=(const Const##NAME& other) {                       \
      BASE_CLASS<Const##NAME, C_IMPL>::operator=(other);                     \
      return *this;                                                          \
    }                                                                        \
                                                                             \
    Const##NAME& operator=(const NAME& other) {                              \
      this->_setPtr(other._ptr());                                           \
      return *this;                                                          \
    }                                                                        \
    Const##NAME& operator=(NAME&& other) {                                   \
      this->_setPtr(other._moveablePtr());                                   \
      return *this;                                                          \
    }                                                                        \
                                                                             \
    using BASE_CLASS<Const##NAME, C_IMPL>::operator==;                       \
    bool operator==(const NAME& other) const {                               \
      return *this == other._ptr();                                          \
    }                                                                        \
    using BASE_CLASS<Const##NAME, C_IMPL>::operator!=;                       \
    bool operator!=(const NAME& other) const {                               \
      return *this != other._ptr();                                          \
    }                                                                        \
                                                                             \
  private:                                                                   \
    Const##NAME(const C_IMPL& p): BASE_CLASS<Const##NAME, C_IMPL>(p) { }     \
    Const##NAME(C_IMPL&& p): BASE_CLASS<Const##NAME, C_IMPL>(std::move(p)) { }\
    friend class CONTAINER;                                                  \
    friend class pistis::typeutil::detail::IteratorRandomAccessOps<Const##NAME, C_IMPL>; \
  };

#define DECLARE_INPUT_ITERATOR(NAME, CONTAINER, IMPL)                        \
  DECLARE_AN_ITERATOR(NAME, CONTAINER, IMPL, pistis::typeutil::InputIterator)

#define DECLARE_OUTPUT_ITERATOR(NAME, CONTAINER, IMPL)                       \
  DECLARE_AN_ITERATOR(NAME, CONTAINER, IMPL, pistis::typeutil::OutputIterator) \

#define DECLARE_FORWARD_ITERATOR(NAME, CONTAINER, IMPL)                      \
  DECLARE_AN_ITERATOR(NAME, CONTAINER, IMPL, pistis::typeutil::ForwardIterator)

#define DECLARE_FORWARD_ITERATORS(NAME, CONTAINER, C_IMPL, M_IMPL)           \
  DECLARE_ITERATOR_PAIR(NAME, CONTAINER, C_IMPL, M_IMPL,                     \
			pistis::typeutil::ForwardIterator)

#define DECLARE_BIDI_ITERATOR(NAME, CONTAINER, IMPL)                         \
  DECLARE_AN_ITERATOR(NAME, CONTAINER, IMPL,                                 \
		      pistis::typeutil::BidirectionalIterator)

#define DECLARE_BIDI_ITERATORS(NAME, CONTAINER, C_IMPL, M_IMPL)              \
  DECLARE_ITERATOR_PAIR(NAME, CONTAINER, C_IMPL, M_IMPL,                     \
			pistis::typeutil::BidirectionalIterator)

#define DECLARE_RANDOM_ACCESS_ITERATOR(NAME, CONTAINER, IMPL)                \
  DECLARE_AN_ITERATOR(NAME, CONTAINER, IMPL,                                 \
                      pistis::typeutil::RandomAccessIterator)

#define DECLARE_RANDOM_ACCESS_ITERATORS(NAME, CONTAINER, C_IMPL, M_IMPL)     \
  DECLARE_ITERATOR_PAIR(NAME, CONTAINER, C_IMPL, M_IMPL,                     \
                        pistis::typeutil::RandomAccessIterator)

#endif
