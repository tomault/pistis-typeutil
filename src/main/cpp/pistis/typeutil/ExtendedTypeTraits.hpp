#ifndef __PISTIS__TYPEUTIL__EXTENDEDTYPETRAITS_HPP__
#define __PISTIS__TYPEUTIL__EXTENDEDTYPETRAITS_HPP__

#include <complex>
#include <type_traits>

namespace pistis {
  namespace typeutil {

    /** @brief std::true_type if T can be copied by copying its bits.
     *
     *  The bit-copyable concept decribes types whose instances can
     *  be copied by copying the instance's bits to another location
     *  in memory.  In other words, given an instance @c x of a type T
     *  and a pointer @c p to a block of memory of size <tt>sizeof(T)</tt>
     *  (which may or may not contain an actual instance of T), executing
     *  <tt>memcpy(p, &x, sizeof(T))</tt> creates a valid copy of @c x
     *  at @c p, with the same effect as <tt>new(p) T(x)</tt> or
     *  <tt>*p = x</tt> depending on whether or not @c p references an
     *  actual instance of T or not.
     *
     *  The purpose for IsBitCopyable is to allow applications to use
     *  memcpy() or memmove() to copy blocks of data around where possible,
     *  which is faster for large blocks than repeatedly invoking the
     *  copy constructor or assignment operator on each instance in
     *  the block.
     */
    template <typename T>
    struct IsBitCopyable : public std::is_pod<T> { };

    template <typename T>
    struct IsBitCopyable< std::complex<T> > : public std::true_type { };

    template <typename T>
    struct RemoveQualifiers :
        public std::remove_cv<typename std::remove_reference<T>::type> {
      // Intentionally left blank
    };

  }
}
#endif

