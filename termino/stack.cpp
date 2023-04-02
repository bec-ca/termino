#include "stack.hpp"

#include "element.hpp"
#include "element_with_position.hpp"

#include <vector>

using std::shared_ptr;
using std::vector;

namespace termino {

Stack::Stack(vector<ElementWithPosition>&& elements, int spacing)
    : _elements(std::move(elements)), _spacing(spacing)
{}

Stack::~Stack() {}

shared_ptr<Stack> Stack::create(
  const vector<Element::ptr>& elements, int spacing)
{
  return make_shared<Stack>(
    vector<ElementWithPosition>(elements.begin(), elements.end()), spacing);
}

void Stack::append(const Element::ptr& element)
{
  append(Element::ptr(element));
}

void Stack::append(Element::ptr&& element)
{
  _elements.push_back(std::move(element));
}

void Stack::clear() { _elements.clear(); }

Size Stack::reflow(const Size& available_space)
{
  int width = 0;
  int height = 0;
  bool first = true;
  for (auto& el : _elements) {
    // TODO: Should deduct the available_space from other elements
    auto s = el.reflow(available_space);
    if (!first) { height += _spacing; }
    first = false;
    el.set_position(height, 0);
    height += s.height;
    width = std::max(width, s.width);
  }

  return Size{.height = height, .width = width};
}

void Stack::draw(Termino& termino, int parent_row, int parent_col) const
{
  for (const auto& el : _elements) { el.draw(termino, parent_row, parent_col); }
}

} // namespace termino
