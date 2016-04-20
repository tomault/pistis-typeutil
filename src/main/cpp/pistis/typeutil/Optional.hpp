#ifndef __PISTIS__TYPEUTIL__OPTIONAL_HPP__
#define __PISTIS__TYPEUTIL__OPTIONAL_HPP__

#include <pistis/exceptions/PistisException.hpp>
#include <new>
#include <ostream>
#include <utility>
#include <stdint.h>

namespace pistis {
  namespace exceptions {
    /** @brief Thrown when the application invokes an operation that
     *         requires a value to be present on an empty Optional.
     */
    class OptionalEmptyError : public PistisException {
    public:
      /** @brief Create a new OptionalEmptyError exception
       *
       *  @param origin  Where the exception originates from
       */
      OptionalEmptyError(const ExceptionOrigin& origin):
	PistisException("Optional is empty", origin) {
      }

      /** @brief Create a copy of this exception, returning a pointer to
       *         a value of the most-derived type
       */
      virtual OptionalEmptyError* duplicate() const {
	return new OptionalEmptyError(*this);
      }
    };
  }
    
  namespace typeutil {

    /** @brief A value that may be absent */
    template <typename T>
    class Optional {
    public:
      /** @brief Creates an empty optional */
      Optional(): present_(false) { }
      
      /** @brief Creates an optional containing a copy of @e v
       *
       *  @param v  Value optional will contain
       */
      explicit Optional(const T& v): present_(true) {
	new(reinterpret_cast<T*>(data_)) T(v);
      }

      /** @brief Creates an optional containing @e v by moving it
       *
       *  @param v  Value optional will contain
       */
      explicit Optional(T&& v): present_(true) {
	new(reinterpret_cast<T*>(data_)) T(std::move(v));
      }

      /** @brief Creates a copy of @e other
       *
       *  Because we declare a move constructor, we need to declare an
       *  explicit copy constructor rather than relying on the template.
       *
       *  @param other  Optional to copy
       */
      Optional(const Optional& other): present_(other.present()) {
	if (present_) {
	  new(reinterpret_cast<T*>(data_)) T(other.value());
	}
      }

      /** @brief Creates a copy of @e other, performing a type conversion
       *         if needed.
       *
       *  @pre  @e U is convertible to @e T
       *  @param other  Optional to copy
       */
      template <typename U>
      Optional(const Optional<U>& other): present_(other.present()) {
	if (present_) {
	  new(reinterpret_cast<T*>(data_)) T(other.value());
	}
      }

      /** @brief Moves @e other into this.
       *
       *  After the move, @e other is empty
       *
       *  @param other  Optional to move
       */
      Optional(Optional&& other): present_(other.present_) {
	if (present_) {
	  new(reinterpret_cast<T*>(data_))
	    T(std::move(*reinterpret_cast<T*>(other.data_)));
	  other.present_ = false;
	}
      }

      /** @brief Destroys this optional */
      ~Optional() { clear(); }

      /** @brief True if this optional does not contain a value */
      bool empty() const { return !present_; }

      /** @brief True if this optional contains a value */
      bool present() const { return present_; }

      /** @brief Return this optional's value
       *
       *  @returns The value contained in this optional
       *  @throws OptionalEmptyError if the optional does not contain a value
       */
      const T& value() const {
	checkForValue_([](){ return PISTIS_EX_HERE; });
	return value_();
      }
	
      /** @brief Return this optional's value
       *
       *  @returns The value contained in this optional
       *  @throws OptionalEmptyError if the optional does not contain a value
       */
      T& value() {
	checkForValue_([]() { return PISTIS_EX_HERE; });
	return value_();
      }

      /** @brief Return this optional's value, if present, or @e defaultValue
       *         if the optional is empty
       *
       *  @returns If the optional contains a value, the contained value,
       *           or @e defaultValue (converted to type T) if the optional
       *           is empty.
       */
      const T& valueOr(const T& defaultValue) const {
	return present() ? value_() : defaultValue;
      }

      /** @brief Return this optional's value, if present, or the result
       *         of calling @e f if the optional is empty.
       *
       *  @param f  Provides the default value if the optional is empty.
       *            Will be called as <em>f()</em>, and should return
       *            value convertible to T.
       *  @returns  the equivalent of
       *            <c>this->present() ? this->value() : f()</c>
       */
      template <typename Function>
      T valueOrCall(Function f) const {
	return present() ? value_() : static_cast<T>(f());
      }

      /** @brief Apply @e f to this optional's value if it has one.
       *
       *  Does nothing if this optional is empty.
       *
       *  @param f  Function to apply.  It should take a single argument of
       *            type T or a type convertible from T.
       *  @returns *this
       */
      template <typename Function>
      const Optional<T>& ifPresent(Function f) const {
	if (present()) {
	  f(value());
	}
	return *this;
      }

      /** @brief Call @e f if this optional is empty.
       *
       *  The @e orElse method allows for constructs like:
       *  <code>
       *     void foo(Optional<T>& opt) {
       *       opt.isPresent([](const T& x) { doSomething(x); }
       *          .orElse([]() { doSomethingElse(); });
       *  </code>
       *
       *  @param f  Function to apply.  It should take no arguments.
       *  @returns  *this
       */
      template <typename Function>
      const Optional<T>& orElse(Function f) const {
	if (empty()) {
	  f();
	}
	return *this;
      }

      /** @brief Apply f to the value of this optional and return an
       *         optional containing the result.
       *
       *  Returns an empty optional of type @e U if this optional is
       *  empty, where @e U is the return type of @e f.
       *
       *  The difference between <c>map(f)</c> and <c>apply(f)</c>
       *  is the former wraps the return value of @e f in an Optional,
       *  while the latter returns the return value of @e f unwrapped.
       *  Furthermore, <c>map(f)</c> returns an empty optional if
       *  invoked on an empty value, while <c>apply(f)</c> returns
       *  <c>U()</c>, where @e U is the return type of @e f.
       *  
       *  @param f  Function to apply.  It should take a value of type
       *            T or a type convertible from T, and must return a value.
       *  @returns  An optional containing the value returned by @e f, or
       *            an empty optional of type @e U, where @e U is the return
       *            type of @e f.
       */
      template <typename Function>
      auto map(Function f) const -> Optional<decltype(f(*(T*)0))> {
	if (present()) {
	  return Optional<decltype(f(*(T*)0))>(f(value_()));
	}
	return Optional<decltype(f(*(T*)0))>();
      }

      /** @brief Apply @e f to the value of this optional and return
       *         the result.
       *
       *  Returns <c>U()</c> if this optional is empty, where @e U is the
       *  return type of @e f.
       *
       *  @param f  Function to apply.  Should take a single argument
       *            of type @e T or a type convertible from @e T.
       *  @returns  The equivalent of
       *            <c>opt.present() ? f(opt.value()) : U()</c>,
       *            where @e U is the return type of @e f.
       */
      template <typename Function>
      auto apply(Function f) const -> decltype(f(*(T*)0)) {
	if (present()) {
	  return f(value_());
	}
	return decltype(f(*(T*)0))();
      }

      /** @brief Apply @f to the value of this optional and return the
       *         result.  Return <c>g()</c> if this optional is empty.
       *
       *  @param f  Function to apply if the optional contains a value.
       *            Should take a single argument of type @e T or convertible
       *            from @e T.
       *  @param g  Function to call if the optional is empty.  It should
       *            take no arguments and return a value convertible to
       *            the return type of @e f.
       *  @returns  A result equivalent to:
       *            <c>opt.present() ? f(opt.value()) : g()</c>
       */
      template <typename PresentFunction, typename AbsentFunction>
      auto applyOr(PresentFunction f, AbsentFunction g) const ->
	decltype(f(*(T*)0)) {
	if (present()) {
	  return f(value_());
	}
	return g();
      }

      /** @brief Returns this optional if <c>p(value())</c> is true, or
       *         an empty optional if this optional is empty or
       *         <c>p(value())</c> is false.
       *
       *  @param p  Predicate used to filter the optional.  It should
       *            take a single argument of type T or convertible to T
       *            and return a value convertible to @e bool.
       *  @returns  <c>*this</c> if this optional is empty or
       *            <c>p(value())</c> is true; an empty optional otherwise.
       */
      template <typename Predicate>
      const Optional<T>& filter(Predicate p) {
	return (empty() || p(value_())) ? *this : emptyOptional_();
      }

      /** @brief Destroy the value this optional contains, leaving it empty.
       *
       *  Does nothing if the optional is empty.
       */
      void clear() {
	if (present()) {
	  reinterpret_cast<T*>(data_)->~T();
	  present_ = false;
	}
      }

      /** @brief True if the optional contains a value. */
      operator bool() const { return present(); }

      /** @brief True if this optional and @e other contain the same value.
       *
       *  @pre   @e T and @e U are equality comparable
       *  @param other  Optional to compare to
       *  @returns  True if this optional and @e other are both empty or
       *            contain values that are equal.
       */
      template <typename U>
      bool operator==(const Optional<U>& other) const {
	if (other.present()) {
	  return present() && (value_() == other.value());
	}
	return empty();
      }

      /** @brief True if this optional and @e other contain different values,
       *         or one is empty while the other isn't.
       *
       *  @pre @e T and @e U are inequality-comparable
       *  @param other Optional to compare to
       *  @returns  True if this optional and @ other contain different
       *            values or one is empty while the other isn't.
       */
      template <typename U>
      bool operator!=(const Optional<U>& other) const {
	if (other.present()) {
	  return empty() || (value_() != other.value());
	}
	return present();
      }

      /** @brief Copy-assign @e other to this optional
       *
       *  Because we declare a move-assignment operator, we need to declare
       *  an explicit copy-assign operator rather than relying on the
       *  template.
       *
       *  @param other  Value to assign
       *  @returns  <c>*this</c>
       */
      Optional& operator=(const Optional& other) {
	if (&other != this) {
	  clear();
	  present_ = other.present();
	  if (present_) {
	    new(reinterpret_cast<T*>(data_)) T(other.value());
	  }
	}
	return *this;
      }

      /** @brief Copy-assign @e other to this optional
       *
       *  @pre  @e U is copy-assignable to @e T
       *  @param other  Value to assign
       *  @returns  <c>*this</c>
       */
      template <typename U>
      Optional& operator=(const Optional<U>& other) {
	// Self-assignment not possible unless the application works
	// very hard
	clear();
	present_ = other.present();
	if (present_) {
	  new(reinterpret_cast<T*>(data_)) T(other.value());
	}
	return *this;
      }

      /** @brief Move-assign @e other to this optional
       *
       *  @e other will be empty after the move.
       *
       *  @param other  Value to move
       *  @returns <c>*this</c>
       */
      Optional& operator=(Optional&& other) {
	if (&other != this) {
	  clear();
	  present_ = other.present();
	  if (present_) {
	    new(reinterpret_cast<T*>(data_)) T(std::move(other.value_()));
	    other.present_ = false;
	  }
	}
	return *this;
      }

    private:
      bool present_; ///< True if the value is present
      alignas(T) uint8_t data_[sizeof(T)]; ///< Object stored here when present

      /** @brief Returns the value contained in this optional.
       *
       *  @pre  <c>present()</c> is true.
       */
      const T& value_() const {
	return *reinterpret_cast<const T*>(data_);
      }

      /** @brief Returns the value contained in this optional.
       *
       *  @pre  <c>present()</c> is true.
       */
      T& value_() { return *reinterpret_cast<T*>(data_); }
      
      /** @brief Return an empty optional of this type */
      static const Optional& emptyOptional_() {
	static const Optional EMPTY;
	return EMPTY;
      }

      /** @brief Verify this optional contains a value
       *
       *  @param origin  No-argument function that returns an
       *                 exceptions::ExceptionOrigin that identifies the
       *                 caller
       *  @throws  OptionalEmptyError  if this optional does not contain
       *           a value.
       */
      template <typename Origin>
      void checkForValue_(Origin origin) const {
	if (empty()) {
	  throw exceptions::OptionalEmptyError(origin());
	}
      }

    };

    template <typename T>
    inline Optional<T> makeOptional(const T& v) { return Optional<T>(v); }

    template <typename T>
    inline Optional<T> makeOptional(T&& v) {
      return Optional<T>(std::move(v));
    }

    template <typename T>
    inline std::ostream& operator<<(std::ostream& out, const Optional<T>& o) {
      if (o.present()) {
	out << o.value();
      }
      return out;
    }
    
  }
}
#endif
