#include "text_box.hpp"

using std::string;

namespace termino {

TextBox::TextBox() : _width(0) {}
TextBox::~TextBox() {}

void TextBox::add_line(const std::vector<Cell>& line)
{
  _content.push_back(line);
  _width = std::max<int>(_width, line.size());
}

void TextBox::add_line(const string& line)
{
  // TODO: Not every char has width 1
  _content.push_back(Cell::of_string(line));
  _width = std::max<int>(_width, line.size());
}

void TextBox::clear()
{
  _content.clear();
  _width = 0;
}

bool TextBox::empty() const { return _content.empty(); }

void TextBox::draw(Termino& termino, int parent_row, int parent_col) const
{
  for (int i = 0; i < int(_content.size()); i++) {
    termino.write_text_at(i + parent_row, parent_col, _content[i]);
  }
}

Size TextBox::reflow(const Size&)
{
  // TODO: Should consider the available space can be smaller
  return Size{.height = int(_content.size()), .width = _width};
}

} // namespace termino
