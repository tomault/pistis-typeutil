#ifndef __PISTIS__TYPEUTIL__HASMEMBER_HPP__
#define __PISTIS__TYPEUTIL__HASMEMBER_HPP__

namespace pistis {
  namespace typeutil {
    namespace detail {
    
      class HasMemberBase {
      protected:
	typedef char (&YesType)[2];
	typedef char (&NoType)[1];
      };

    }
  }
}

#define DECLARE_HAS_MEMBER_TYPE(NAME, TYPE_NAME)                             \
  template <typename T>                                                      \
  class NAME : public pistis::typeutil::detail::HasMemberBase {	             \
  private:                                                                   \
    template <typename U> struct filter { };                                 \
                                                                             \
    template <typename U> static YesType check(filter<typename U::TYPE_NAME>*); 	\
    template <typename U> static NoType check(...);                          \
  public:                                                                    \
    enum { value= sizeof(check<T>(0)) == sizeof(YesType) };                  \
  };

#define DECLARE_HAS_MEMBER_VAR(NAME, VAR_NAME, VAR_TYPE)                     \
  template <typename T>                                                      \
  class NAME : public pistis::typeutil::detail::HasMemberBase {	             \
  private:                                                                   \
    template <VAR_TYPE U::*> struct filter { };                              \
                                                                             \
    template <typename U> static YesType check(filter<&U::VAR_NAME>*);       \
    template <typename U> static NoType check(...);                          \
  public:                                                                    \
    enum { value= sizeof(check<T>(0)) == sizeof(YesType) };                  \
  };

#define DECLARE_HAS_MEMBER_FN(NAME, FN_NAME, RET_TYPE, ...)                  \
  template <typename T>                                                      \
  class NAME : public pistis::typeutil::detail::HasMemberBase {	             \
  private:                                                                   \
    template <RET_TYPE (U::*)(__VA_ARGS__)> struct filter { };               \
                                                                             \
    template <typename U> static YesType check(filter<&U::FN_NAME>*);	     \
    template <typename U> static NoType check(...);                          \
  public:                                                                    \
    enum { value= sizeof(check<T>(0)) == sizeof(NoType) };                   \
  };
#endif
