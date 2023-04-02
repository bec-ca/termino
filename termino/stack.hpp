#pragma once

#include "element.hpp"
#include "element_with_position.hpp"

namespace termino {

struct Stack : public Element {
 public:
  using ptr = std::shared_ptr<Stack>;

  Stack(std::vector<ElementWithPosition>&& elements = {}, int spacing = 0);
  virtual ~Stack();

  static std::shared_ptr<Stack> create(
    const std::vector<Element::ptr>& elements = {}, int spacing = 0);

  void append(const Element::ptr& element);
  void append(Element::ptr&& element);

  void clear();

  virtual Size reflow(const Size& available_space);

  virtual void draw(Termino& termino, int parent_row, int parent_col) const;

 private:
  std::vector<ElementWithPosition> _elements;
  const int _spacing;
};

} // namespace termino
