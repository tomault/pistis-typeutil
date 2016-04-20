#ifndef __PISTIS__TYPEUTIL__NAMEOF_HPP__
#define __PISTIS__TYPEUTIL__NAMEOF_HPP__

#include <string>
#include <typeinfo>
#include <cxxabi.h>

namespace pistis {
  namespace typeutil {

    /** @brief Returns a printable name for type T */
    template <typename T>
    std::string nameOf() {
      int status;
      char* demangled= abi::__cxa_demangle(typeid(T).name(), 0, 0, &status);
      std::string result(demangled);
      free(demangled);
      return result;
    }

  }
}
#endif

