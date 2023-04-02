#pragma once

#include "element.hpp"
#include "element_with_position.hpp"

namespace termino {

struct Margin : public Element {
 public:
  virtual ~Margin();

  static Element::ptr create(const Element::ptr el, int margin);

  Margin(const Element::ptr el, int margin);

  virtual Size reflow(const Size& available_space);

  virtual void draw(Termino& termino, int parent_row, int parent_col) const;

 private:
  ElementWithPosition _element;
  const int _margin;
};

} // namespace termino
