#pragma once

#include "element.hpp"

namespace termino {

struct TextBox : public Element {
  using ptr = std::shared_ptr<TextBox>;
  TextBox();
  ~TextBox();

  void add_line(const std::vector<Cell>& line);

  void add_line(const std::string& line);

  void clear();

  bool empty() const;

  virtual void draw(Termino& termino, int parent_row, int parent_col) const;

  virtual Size reflow(const Size& available_space);

 private:
  std::vector<std::vector<Cell>> _content;
  int _width;
};

} // namespace termino
