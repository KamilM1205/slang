#include "panic.hpp"
#include "message.hpp"
#include <cstdlib>
#include <iostream>

void panic() {
  std::cout << MessageContainer::get_instance()->to_string() << std::endl;
  std::cout << "A fatal error has occurred. Panic!" << std::endl;
  exit(EXIT_FAILURE);
}
