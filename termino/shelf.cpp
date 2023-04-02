#include "shelf.hpp"

#include "element.hpp"
#include "element_with_position.hpp"

using std::shared_ptr;
using std::vector;

namespace termino {

Shelf::Shelf(vector<ElementWithPosition>&& elements, int spacing)
    : _elements(std::move(elements)), _spacing(spacing)
{}

shared_ptr<Shelf> Shelf::create(
  const vector<Element::ptr>& elements, int spacing)
{
  return make_shared<Shelf>(
    vector<ElementWithPosition>(elements.begin(), elements.end()), spacing);
}

Shelf::~Shelf() {}

void Shelf::append(const Element::ptr element) { _elements.push_back(element); }

Size Shelf::reflow(const Size& available_space)
{
  int width = 0;
  int height = 0;
  for (auto& el : _elements) {
    // TODO: Should only pass in the actual available space discounting other
    // elements
    auto s = el.reflow(available_space);
    if (width > 0) { width += _spacing; }
    el.set_position(0, width);
    width += s.width;
    height = std::max(height, s.height);
  }

  return Size{.height = height, .width = width};
}

void Shelf::draw(Termino& termino, int parent_row, int parent_col) const
{
  for (const auto& el : _elements) { el.draw(termino, parent_row, parent_col); }
}

} // namespace termino
