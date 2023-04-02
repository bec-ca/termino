#include "element_with_position.hpp"

namespace termino {

ElementWithPosition::ElementWithPosition(const Element::ptr element)
    : _element(element)
{}

ElementWithPosition::~ElementWithPosition() {}

void ElementWithPosition::draw(
  Termino& termino, int parent_row, int parent_col) const
{
  _element->draw(termino, parent_row + _row, parent_col + _col);
}

void ElementWithPosition::set_position(int row, int col)
{
  _row = row;
  _col = col;
}

Size ElementWithPosition::reflow(const Size& available_space)
{
  return _element->reflow(available_space);
}

} // namespace termino
