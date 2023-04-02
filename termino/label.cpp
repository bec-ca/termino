#include "label.hpp"

using std::string;
using std::vector;

namespace termino {

Label::Label() {}
Label::Label(string&& content) : _content(std::move(content)) {}
Label::Label(const string& content) : Label(string(content)) {}

Label::~Label() {}

void Label::set_text(std::string&& text) { _content = std::move(text); }

void Label::set_text(const std::string& text) { set_text(string(text)); }

void Label::draw(Termino& termino, int parent_row, int parent_col) const
{
  vector<Cell> text;
  for (int i = 0; i < _available_width; i++) {
    char c = i < int(_content.size()) ? _content[i] : ' ';
    text.push_back(
      Cell::char_with_color_and_background(c, -1, _background_color));
  }
  if (!text.empty()) { termino.write_text_at(parent_row, parent_col, text); }
}

void Label::set_background_color(int color) { _background_color = color; }

Size Label::reflow(const Size& available_space)
{
  _available_width = available_space.height > 0 ? available_space.width : 0;
  return Size{.height = 1, .width = available_space.width};
}

} // namespace termino
