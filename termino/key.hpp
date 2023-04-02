#pragma once

#include "key_code.hpp"

#include <string>

namespace termino {

struct Key {
  int character;
  KeyCode key_code;
  bool ctrl_pressed;
  bool shift_pressed;

  std::string raw;
  std::string name;

  std::string repr();
};

} // namespace termino
