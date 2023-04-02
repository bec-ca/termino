#pragma once

#include "termino.hpp"

#include <memory>

namespace termino {

struct Element {
 public:
  using ptr = std::shared_ptr<Element>;
  virtual ~Element();

  virtual void draw(Termino& termino, int parent_row, int parent_col) const = 0;

  virtual Size reflow(const Size& available_space) = 0;
};

} // namespace termino
