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

    /** @brief Remove const, volatile and references from a type
     *
     *  Equivalent to combining std::remove_cv and std::remove_reference.
     */
    template <typename T>
    struct RemoveCVR :
        public std::remove_cv<typename std::remove_reference<T>::type> {
      // Intentionally left blank
    };

    /** @brief Remove const, volatile and references from a type
     *
     *  Equivalent to combining std::remove_cv and std::remove_reference.
     *  @deprecated  Use RemoveCVR instead
     */
    template <typename T>
    struct [[ deprecated("Use RemoveCVR instead")]] RemoveQualifiers :
        public RemoveCVR<T> {
    };

    /** @brief True if T is a character type
     *
     *  The IsCharType metafunction recognizes the following built-in
     *  types as character types: char, wchar_t char16_t and char32_t.
     *  Because unsigned char and signed char are typically used to
     *  represent signed and unsigned bytes respectively, IsCharType
     *  does not consider them to be "characters."  Applications may add
     *  additonal specializations.
     */
    template <typename T>
    struct IsCharType : public std::false_type { };

    template<>
    struct IsCharType<char> : public std::true_type { };

    template<>
    struct IsCharType<wchar_t> : public std::true_type { };

#ifdef __cpp_char8_t
    template<>
    struct IsCharType<char8_t> : public std::true_type { };
#endif

    template<>
    struct IsCharType<char16_t> : public std::true_type { };

    template<>
    struct IsCharType<char32_t> : public std::true_type { };

  }
}
#endif

