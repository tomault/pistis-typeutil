#ifndef __PISTIS__TYPEUTIL__LAMBDAOVERLOAD_HPP__
#define __PISTIS__TYPEUTIL__LAMBDAOVERLOAD_HPP__

#include <utility>

namespace pistis {
  namespace typeutil {
    namespace detail {

      template <typename... Functions> struct OverloadSet;
      
      template <typename F, typename... Functions>
      struct OverloadSet<F, Functions...> : F, OverloadSet<Functions...>::Type {
	typedef OverloadSet Type;
	
	OverloadSet(F f, Functions... functions):
	  F(f), OverloadSet<Functions...>::Type(functions...) {
	}

	using F::operator();
	using OverloadSet<Functions...>::Type::operator();
      };

      template <typename F>
      struct OverloadSet<F> : F {
	typedef F Type;

	OverloadSet(F f) : F(f) { }
	using F::operator();
      };
    }

    template <typename... Functions>
    inline auto overloadLambda(Functions... functions) {
      return detail::OverloadSet<Functions...>(functions...);
    }
    
  }
}
#endif
