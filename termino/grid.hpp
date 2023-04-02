#pragma once

#include "element.hpp"
#include "element_with_position.hpp"

namespace termino {

struct Grid : public Element {
 public:
  virtual ~Grid();

  void append_to_row(int row, const Element::ptr element);

  virtual Size reflow(const Size& available_space);

  void draw(Termino& termino, int parent_row, int parent_col) const;

 private:
  std::vector<std::vector<ElementWithPosition>> _grid;
  int _num_cols = 0;
};

} // namespace termino
