#pragma once

#include <string>
#include <vector>

namespace termino {

struct Cell {
  int chr;
  int background = -1;
  int foreground = -1;

  static Cell default_color(int chr);

  static Cell char_with_color(int chr, int color);

  static Cell char_with_color_and_background(
    int chr, int color, int background);

  static Cell space();

  bool operator==(const Cell& other) const;

  static std::vector<Cell> of_string(const std::string& str);

  static std::vector<Cell> of_string_with_color(
    const std::string& str, int color);

  static std::vector<Cell> of_string_with_color_and_background(
    const std::string& str, int color, int background);
};

} // namespace termino
