#include "termino.hpp"

#include "bee/error.hpp"
#include "bee/file_reader.hpp"
#include "bee/format.hpp"
#include "bee/string_util.hpp"
#include "command/command_builder.hpp"
#include "command/group_builder.hpp"

#include <optional>

using bee::format;
using std::string;
using std::vector;

namespace termino {
namespace {

bee::OrError<bee::Unit> show_keys()
{
  Termino termino;
  bail_unit(termino.make_raw_mode());

  int line = 1;

  while (true) {
    bail(input, termino.read_input());
    auto size = termino.get_window_size();
    if (line >= size.height) {
      termino.erase_screen();
      line = 1;
    }
    termino.write_text_at(
      0,
      0,
      Cell::of_string(format("width: $ height: $", size.width, size.height)));
    if (!input.has_value()) continue;
    if (input->character == 'q' || input->character == 'Q') break;
    string text;
    text = input->repr();
    termino.write_text_at(line, 0, Cell::of_string(text));
    line++;
    bail_unit(termino.draw());
  }

  return bee::ok();
}

bee::OrError<bee::Unit> run_less(const std::optional<string>& filename)
{
  Termino termino;
  bail_unit(termino.make_raw_mode());

  bail(data, bee::FileReader::read_file(bee::FilePath::of_string(*filename)));
  auto content = bee::split(data, "\n");

  int line_offset = 0;

  vector<Cell> line;

  while (true) {
    auto window_size = termino.get_window_size();
    termino.erase_screen();
    for (int i = 0;
         i < std::min<int>(content.size() - line_offset, window_size.height);
         i++) {
      int line_num = i + line_offset;
      const auto& line_content = content[line_num];
      int background = line_num % 2 == 0 ? -1 : 236;
      line.clear();
      for (char c : line_content) {
        int color = (c == ',') ? 11 : 247;
        line.push_back(
          Cell::char_with_color_and_background(c, color, background));
      }
      if (background != -1) {
        while (line.size() < size_t(window_size.width)) {
          line.push_back(
            Cell::char_with_color_and_background(' ', -1, background));
        }
      }
      termino.write_text_at(i, 0, line);
    }
    bail_unit(termino.draw());
    bail(input, termino.read_input());
    if (!input.has_value()) continue;
    if (
      (!input->ctrl_pressed &&
       (input->character == 'q' || input->character == 'Q')) ||
      input->key_code == KeyCode::Escape) {
      break;
    } else if (input->key_code == KeyCode::Up) {
      line_offset = std::max(line_offset - 1, 0);
    } else if (input->key_code == KeyCode::Down) {
      line_offset =
        std::min<int>(line_offset + 1, content.size() - window_size.height);
    } else if (input->key_code == KeyCode::PgUp) {
      line_offset = std::max(line_offset - (window_size.height / 2), 0);
    } else if (input->key_code == KeyCode::PgDown) {
      line_offset = std::min<int>(
        line_offset + (window_size.height / 2),
        content.size() - window_size.height);
    }
  }

  return bee::ok();
}

bee::OrError<bee::Unit> run_editor(const std::optional<string>& filename)
{
  Termino termino;
  bail_unit(termino.make_raw_mode());

  vector<string> document;
  if (filename.has_value()) {
    bail(data, bee::FileReader::read_file(bee::FilePath::of_string(*filename)));
    document = bee::split(data, "\n");
  }

  int view_offset = 0;
  vector<Cell> line_buffer;

  while (true) {
    auto window_size = termino.get_window_size();
    termino.erase_screen();
    for (int i = 0;
         i < std::min<int>(document.size() - view_offset, window_size.height);
         i++) {
      int line_num = i + view_offset;
      const auto& doc_line = document[line_num];
      line_buffer.clear();
      for (char c : doc_line) { line_buffer.push_back(Cell::default_color(c)); }
      termino.write_text_at(i, 0, line_buffer);
    }
    bail_unit(termino.draw());
    bail(input, termino.read_input());
    if (!input.has_value()) continue;
    if (input->key_code == KeyCode::Escape) {
      break;
    } else if (input->key_code == KeyCode::Up) {
      view_offset--;
    } else if (input->key_code == KeyCode::Down) {
      view_offset++;
    } else if (input->key_code == KeyCode::PgUp) {
      view_offset -= window_size.height / 2;
    } else if (input->key_code == KeyCode::PgDown) {
      view_offset += window_size.height / 2;
    }
    int cap = std::max(0, int(document.size()) - window_size.height + 1);
    view_offset = std::clamp(view_offset, 0, cap);
  }

  return bee::ok();
}

using command::Cmd;
using command::CommandBuilder;
using command::GroupBuilder;

Cmd edit_command()
{
  using namespace command::flags;
  auto builder = CommandBuilder("Editor");
  auto path = builder.anon(string_flag, "file");
  return builder.run(
    [=]() -> bee::OrError<bee::Unit> { return run_editor(*path); });
}

Cmd less_command()
{
  using namespace command::flags;
  auto builder = CommandBuilder("Less");
  auto path = builder.anon(string_flag, "file");
  return builder.run(
    [=]() -> bee::OrError<bee::Unit> { return run_less(*path); });
}

Cmd show_keys_command()
{
  using namespace command::flags;
  auto builder = CommandBuilder("Show keys");
  return builder.run([=]() -> bee::OrError<bee::Unit> { return show_keys(); });
}

int main(int argc, char** argv)
{
  return GroupBuilder("Termino demos")
    .cmd("less", less_command())
    .cmd("edit", edit_command())
    .cmd("show-keys", show_keys_command())
    .build()
    .main(argc, argv);
}

} // namespace
} // namespace termino

int main(int argc, char** argv) { return termino::main(argc, argv); }
