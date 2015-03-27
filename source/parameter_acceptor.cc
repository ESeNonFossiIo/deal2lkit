#include "parameter_acceptor.h"
#include <cxxabi.h>

namespace {
  
  struct handle {
    char* p;
    handle(char* ptr) : p(ptr) { }
    ~handle() { std::free(p); }
  };
  
  std::string demangle(const char* name) {
    int status = -4; // some arbitrary value to eliminate the compiler warning
    handle result( abi::__cxa_demangle(name, NULL, NULL, &status) );
    return (status==0) ? result.p : name ;
  };
  
  template <class T>
  std::string type(const T& t) {
    return demangle(typeid(t).name());
  };

}


// Static empty class list
std::vector<SmartPointer<ParameterAcceptor> > ParameterAcceptor::class_list;
std::vector<std::string> ParameterAcceptor::section_names;


ParameterAcceptor::ParameterAcceptor(const std::string name) :
  acceptor_id(class_list.size()),
  section_name(name)
{
  SmartPointer<ParameterAcceptor> pt(this, type(*this).c_str());
  class_list.push_back(pt);
}
  

ParameterAcceptor::~ParameterAcceptor() {
  class_list[acceptor_id] = 0;
}

std::string ParameterAcceptor::get_section_name() const {
  return (section_name != "" ? section_name : type(*this));
}


void ParameterAcceptor::parse_all_parameters(ParameterHandler &prm) {
  for(unsigned int i=0; i< class_list.size(); ++i) {
    prm.enter_subsection(class_list[i]->get_section_name());
    class_list[i]->parse_parameters(prm);
    prm.leave_subsection();
  }
}

void ParameterAcceptor::declare_all_parameters(ParameterHandler &prm) {
  for(unsigned int i=0; i< class_list.size(); ++i) {
    prm.enter_subsection(class_list[i]->get_section_name());
    class_list[i]->declare_parameters(prm);
    prm.leave_subsection();
  }
}
