#ifndef SCOPE_HPP
#define SCOPE_HPP

#include "types.hpp"
#include <string>
#include <unordered_map>

class Scope {
  std::unordered_map<std::string, Var> var_list;

public:
  Scope();
};

#endif // !SCOPE_HPP
