#pragma once

#include "element.hpp"

namespace termino {

struct Label : public Element {
  using ptr = std::shared_ptr<Label>;
  Label();
  Label(const std::string& content);
  Label(std::string&& content);
  ~Label();

  void set_text(const std::string& text);
  void set_text(std::string&& text);

  void set_background_color(int color);

  virtual void draw(Termino& termino, int parent_row, int parent_col) const;

  virtual Size reflow(const Size& available_space);

 private:
  std::string _content;
  int _background_color = -1;
  int _available_width = 0;
};

} // namespace termino
