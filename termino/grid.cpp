#include "grid.hpp"

#include "element.hpp"
#include "element_with_position.hpp"

using std::vector;

namespace termino {

Grid::~Grid() {}

void Grid::append_to_row(int row, const Element::ptr element)
{
  if (row >= int(_grid.size())) { _grid.resize(row + 1); }
  _grid[row].push_back(element);
  _num_cols = std::max<int>(_num_cols, _grid[row].size());
}

Size Grid::reflow(const Size& available_space)
{
  vector<int> column_widths(_num_cols, 0);
  vector<int> row_heights(_grid.size(), 0);
  {
    for (int i = 0; i < int(_grid.size()); i++) {
      auto& line = _grid[i];
      for (int j = 0; j < int(line.size()); j++) {
        auto& el = line[j];
        // TODO: should only pass the maximum this cell can have
        auto s = el.reflow(available_space);
        column_widths[j] = std::max(column_widths[j], s.width);
        row_heights[j] = std::max(row_heights[j], s.height);
      }
    }
  }
  {
    int row = 0;
    for (int i = 0; i < int(_grid.size()); i++) {
      auto& line = _grid[i];
      int col = 0;
      for (int j = 0; j < int(line.size()); j++) {
        auto& el = line[j];
        el.set_position(row, col);
        col += column_widths[j];
      }
      row += row_heights[i];
    }
  }

  int height = 0;
  int width = 0;
  for (int h : row_heights) height += h;
  for (int w : column_widths) width += w;
  return Size{.height = height, .width = width};
}

void Grid::draw(Termino& termino, int parent_row, int parent_col) const
{
  for (const auto& row : _grid) {
    for (const auto& el : row) { el.draw(termino, parent_row, parent_col); }
  }
}

} // namespace termino
