#include "cell.hpp"

namespace termino {

Cell Cell::default_color(int chr)
{
  return Cell{.chr = chr, .background = -1, .foreground = -1};
}

Cell Cell::char_with_color(int chr, int color)
{
  return Cell{.chr = chr, .background = -1, .foreground = color};
}

Cell Cell::char_with_color_and_background(int chr, int color, int background)
{
  return Cell{.chr = chr, .background = background, .foreground = color};
}

Cell Cell::space() { return default_color(' '); }

bool Cell::operator==(const Cell& other) const
{
  return other.chr == chr && other.background == background &&
         other.foreground == foreground;
}

std::vector<Cell> Cell::of_string(const std::string& str)
{
  return of_string_with_color(str, -1);
}

std::vector<Cell> Cell::of_string_with_color(const std::string& str, int color)
{
  return of_string_with_color_and_background(str, color, -1);
}

std::vector<Cell> Cell::of_string_with_color_and_background(
  const std::string& str, int color, int background)
{
  std::vector<Cell> cells;
  for (char c : str) {
    if (c < 32) { c = 32; }
    cells.push_back(
      Cell{.chr = c, .background = background, .foreground = color});
  }
  return cells;
}

} // namespace termino
