#ifndef __PISTIS__TYPEUTIL__ALLOCATORUTIL_HPP__
#define __PISTIS__TYPEUTIL__ALLOCATORUTIL_HPP__

#include <allocator_traits>
#include <utility>

namespace pistis {
  namespace typeutil {

    template <typename A>
    inline A propagateOnCopyAssignment(const A& a) {
      return std::allocator_traits<A>
	  ::propagate_on_container_copy_assignment::value ? a : A();
    }

    template <typename A>
    inline A propagateOnMoveAssignment(A&& a) {
      return std::allocator_traits<A>
	::propagate_on_container_move_assignment::value ? std::move(a) : A();
    }

    template <typename A>
    inline void swapAllocators(A& left, A& right) {
      if (std::allocator_traits<A>::propagate_on_container_swap::value) {
	std::swap(left, right);
      }
    }
    
    
  }
}
#endif
