#ifndef __PISTIS__TYPEUTIL__ENUM_HPP__
#define __PISTIS__TYPEUTIL__ENUM_HPP__

#include <pistis/typeutil/NameOf.hpp>
#include <pistis/exceptions/NoSuchItem.hpp>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace pistis {
  namespace typeutil {

    template <typename DerivedT, typename ImplT>
    class BasicEnumMemberData {
    public:
      BasicEnumMemberData() { }
      virtual ~BasicEnumMemberData() {
	for (auto i= _impls.begin(); i != _impls.end(); ++i) {
	  delete *i;
	}
      }

      virtual void add(ImplT* impl, const DerivedT& dv) {
	_valueToMember.insert(std::make_pair(impl->value(), dv));
	_nameToMember.insert(std::make_pair(impl->name(), dv));
	_members.push_back(dv);
	_impls.push_back(impl);
      }

      DerivedT fromName(const std::string& name) const {
	typename std::map<std::string, DerivedT>::const_iterator i=
	  _nameToMember.find(name);
	if (i == _nameToMember.end()) {
	  std::ostringstream msg;
	  msg << "Member of " << nameOf<DerivedT>() << " with name \""
	      << name << "\"";
	  throw exceptions::NoSuchItem(msg.str(), PISTIS_EX_HERE);
	}
	return i->second;
      }

      DerivedT fromValue(typename ImplT::ValueType value) const {
	typename std::map<typename ImplT::ValueType, DerivedT>::const_iterator
	  i= _valueToMember.find(value);
	if (i == _valueToMember.end()) {
	  std::ostringstream msg;
	  msg << "Member of " << nameOf<DerivedT>() << " with value "
	      << value;
	  throw exceptions::NoSuchItem(msg.str(), PISTIS_EX_HERE);
	}
	return i->second;
      }

      const std::vector<DerivedT>& values() const { return _members; }

    private:
      std::map<typename ImplT::ValueType, DerivedT> _valueToMember;
      std::map<std::string, DerivedT> _nameToMember;
      std::vector<DerivedT> _members;	
      std::vector<ImplT*> _impls;
    };

    template <typename BaseTypeT>
    class BasicEnumImpl {
      BasicEnumImpl(const BasicEnumImpl<BaseTypeT>& other) = delete;
      BasicEnumImpl<BaseTypeT>& operator=(const BasicEnumImpl<BaseTypeT>& other) = delete;

    public:
      typedef BaseTypeT ValueType;

      template <typename DerivedT, typename ImplT>
      struct MemberData {
	typedef BasicEnumMemberData<DerivedT, ImplT> type;
      };

    public:
      BasicEnumImpl(BaseTypeT value, const std::string& name):
	_value(value), _name(name) {
      }
      BasicEnumImpl(BaseTypeT value, std::string&& name):
	_value(value), _name(std::move(name)) {
      }
	  
      ValueType value() const { return _value; }
      const std::string& name() const { return _name; }
	  
      template <typename DerivedT, typename ImplT>
      static BasicEnumMemberData<DerivedT, ImplT>* createMemberData() {
	return new BasicEnumMemberData<DerivedT, ImplT>;
      }

    private:
      BaseTypeT _value;
      std::string _name;
    };

    template <typename DerivedT, typename ImplT = BasicEnumImpl<int> >
    class Enum {
    public:
      Enum(const Enum<DerivedT, ImplT>& other): _impl(other._impl) { }
      Enum(Enum<DerivedT, ImplT>&& other): _impl(other._impl) { }
      Enum(const DerivedT& other): _impl(other._impl) { }
      Enum(DerivedT&& other): _impl(other._impl) { }
      
      typename ImplT::ValueType value() const { return _impl->value(); }
      const std::string& name() const { return _impl->name(); }

      Enum& operator=(const Enum&)= default;
      Enum& operator=(Enum&&)= default;

      bool operator==(const DerivedT& other) const {
	return _impl == other._impl;
      }
      bool operator!=(const DerivedT& other) const {
	return _impl != other._impl;
      }
      bool operator<(const DerivedT& other) const {
	return value() < other.value();
      }
      bool operator>(const DerivedT& other) const {
	return value() > other.value();
      }
      bool operator<=(const DerivedT& other) const {
	return value() <= other.value();
      }
      bool operator>=(const DerivedT& other) const {
	return value() >= other.value();
      }

      static DerivedT fromValue(typename ImplT::ValueType value) {
	return _members->fromValue(value);
      }

      static DerivedT fromName(const std::string& name) {
	return _members->fromName(name);
      }

      static const std::vector<DerivedT>& values() {
	return _members->values();
      }

    protected:
      typedef typename ImplT::template MemberData<DerivedT, ImplT>::type
              MemberDataType;

      template <typename... Args>
      Enum(Args&&... args): _impl(new ImplT(std::forward<Args>(args)...)) {
	DerivedT& dv= static_cast<DerivedT&>(*this);
	if (!_members) {
	  _members= ImplT::template createMemberData<DerivedT, ImplT>();
	}
	_members->add(_impl, dv);
      }
      Enum(ImplT* impl): _impl(impl) { }
      
      ImplT* _getImpl() const { return _impl; }
      void _setImpl(ImplT* impl) { _impl= impl; }
      static MemberDataType* _getMembers() { return _members; }
      static void _setMembers(MemberDataType* m) { _members= m; }

    private:
      ImplT* _impl;

      class Destructor {
      public:
	~Destructor() { delete _members; }
      };

      static MemberDataType* _members;
      static Destructor _destructor;
      friend class Destructor;
    };

    template <typename DerivedT, typename ImplT>
    typename Enum<DerivedT, ImplT>::MemberDataType*
      Enum<DerivedT, ImplT>::_members= nullptr;

    template <typename DerivedT, typename ImplT>
    typename Enum<DerivedT, ImplT>::Destructor
      Enum<DerivedT, ImplT>::_destructor;

    template <typename DerivedT, typename ImplT>
    inline std::ostream& operator<<(std::ostream& out,
				    const Enum<DerivedT, ImplT>& e) {
      return out << e.name();
    }

  }
}
#endif
