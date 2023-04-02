#pragma once

#include "element.hpp"

namespace termino {

struct ElementWithPosition {
 public:
  ElementWithPosition(const Element::ptr element);
  ~ElementWithPosition();

  void draw(Termino& termino, int parent_row, int parent_col) const;

  void set_position(int row, int col);

  Size reflow(const Size& available_space);

 private:
  int _row = 0;
  int _col = 0;
  Element::ptr _element;
};

} // namespace termino
