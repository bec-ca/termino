#pragma once

#include "element.hpp"
#include "element_with_position.hpp"

namespace termino {

struct Shelf : public Element {
 public:
  Shelf(std::vector<ElementWithPosition>&& elements, int spacing);

  static std::shared_ptr<Shelf> create(
    const std::vector<Element::ptr>& elements = {}, int spacing = 0);

  virtual ~Shelf();

  void append(const Element::ptr element);

  virtual Size reflow(const Size& available_space);

  virtual void draw(Termino& termino, int parent_row, int parent_col) const;

 private:
  std::vector<ElementWithPosition> _elements;
  const int _spacing;
};

} // namespace termino
