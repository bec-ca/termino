#include "margin.hpp"

#include "element.hpp"
#include "element_with_position.hpp"

namespace termino {

Margin::~Margin() {}

Element::ptr Margin::create(const Element::ptr el, int margin)
{
  auto m = make_shared<Margin>(el, margin);
  return m;
}

Margin::Margin(const Element::ptr el, int margin)
    : _element(el), _margin(margin)
{}

Size Margin::reflow(const Size& available_space)
{
  int offset_x = _margin;
  int offset_y = _margin * 2;

  int added_height = offset_x * 2;
  int added_width = offset_y * 2;

  auto s = _element.reflow(Size{
    .height = available_space.height - added_height,
    .width = available_space.width - added_width});
  _element.set_position(offset_x, offset_y);
  return Size{
    .height = s.height + added_height, .width = s.width + added_width};
}

void Margin::draw(Termino& termino, int parent_row, int parent_col) const
{
  _element.draw(termino, parent_row, parent_col);
}

} // namespace termino
