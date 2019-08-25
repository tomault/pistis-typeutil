#ifndef __PISTIS__TYPEUTIL__INVOKEWITHTUPLE_HPP__
#define __PISTIS__TYPEUTIL__INVOKEWITHTUPLE_HPP__

#include <tuple>
#include <utility>
#include <stddef.h>

namespace pistis {
  namespace typeutil {
    namespace detail {

      /** @brief Helper function for invokeWithTuple()
       *
       *  invokeWithTuple() calls this function with its arguments plus
       *  a std::index_sequence from 0 to N - 1, where N is the number of
       *  arguments, so that the arguments from the tuple can be unpacked
       *  in order by unpacking std::get<Index>(args).
       *
       *  @param f     The function to call
       *  @param args  Its arguments, wrapped in a std::tuple
       *  @returns     f(args)
       *  @throws      Whatever f throws
       */
      template <typename Function, typename Tuple, size_t... Indices>
      auto invokeWithTuple_(Function&& f, Tuple&& args,
			    std::index_sequence<Indices...>) {
	return std::forward<Function>(f)(
	    std::get<Indices>(std::forward<Tuple>(args))...
	);
      }
    }

    template <typename Function, typename... Args>
    auto invokeWithTuple(Function&& f, const std::tuple<Args...>& args) {
      return detail::invokeWithTuple_(
	  std::forward<Function>(f), args, std::index_sequence_for<Args...>()
      );
    }
    
    /** @brief Call a function with arguments contained in a tuple
     *
     *  @param f    The function to call
     *  @param args Arguments for f
     *  @returns    f(args)
     *  @throws     Whatever f throws
     */
    //template <typename Function, typename... Args>
    //auto invokeWithTuple(Function&& f, std::tuple<Args...>&& args) {
    //  return detail::invokeWithTuple_(
    //	  std::forward<Function>(f),
    //	  std::forward< std::tuple<Args...> >(args),
    //	  std::index_sequence_for<Args...>()
    //);
    //}
      
  }
}

#endif
