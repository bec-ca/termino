#include "termino.hpp"

#include "constants.hpp"

#include "bee/format_vector.hpp"

#include <cerrno>
#include <cstring>
#include <memory>
#include <optional>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#include <unordered_map>

using bee::format;
using std::make_unique;
using std::nullopt;
using std::optional;
using std::string;
using std::unordered_map;
using std::vector;

namespace termino {

namespace {

unordered_map<string, Key> create_key_table()
{
  unordered_map<string, Key> table;
  auto printable = [&](char c, bool shift) {
    string str;
    str += c;
    table[str] = Key{
      .character = c,
      .key_code = KeyCode::Printable,
      .ctrl_pressed = false,
      .shift_pressed = shift,
      .raw = str,
      .name = format("Printable($)", str),
    };
  };
  for (int c = 32; c <= 126; c++) { printable(c, false); }
  for (int c = 'A'; c <= 'Z'; c++) { printable(c, true); }

  auto control_printable = [&](char c, char disp) {
    string str;
    str += c;
    table[str] = Key{
      .character = disp,
      .key_code = KeyCode::Printable,
      .ctrl_pressed = true,
      .shift_pressed = false,
      .raw = str,
      .name = format("Control($)", disp),
    };
  };
  for (int c = 1; c <= 26; c++) { control_printable(c, c - 1 + 'A'); }
  for (int c = 28; c <= 31; c++) { control_printable(c, c - 28 + '4'); }

  auto control = [&](char c, KeyCode code, const string& name) {
    string str;
    str += c;
    table[str] = Key{
      .character = c,
      .key_code = code,
      .ctrl_pressed = false,
      .shift_pressed = false,
      .raw = str,
      .name = name,
    };
  };

  control(escape_char, KeyCode::Escape, "Esc");
  control(0, KeyCode::Escape, "Esc");
  control(9, KeyCode::Tab, "Tab");
  control(13, KeyCode::Enter, "Enter");
  control(127, KeyCode::Backspace, "Backspace");

  auto seq = [&](string suffix, KeyCode code, const string& name) {
    string str;
    str += escape_char;
    str += control_seq_char;
    str += suffix;
    table[str] = Key{
      .character = 0,
      .key_code = code,
      .ctrl_pressed = false,
      .shift_pressed = false,
      .raw = str,
      .name = name,
    };
  };
  seq("A", KeyCode::Up, "Up");
  seq("B", KeyCode::Down, "Down");
  seq("C", KeyCode::Right, "Right");
  seq("D", KeyCode::Left, "Left");

  seq("1~", KeyCode::Home, "Home");
  seq("3~", KeyCode::Del, "Del");
  seq("4~", KeyCode::End, "End");
  seq("5~", KeyCode::PgUp, "PgUp");
  seq("6~", KeyCode::PgDown, "PgDown");

  return table;
}

const unordered_map<string, Key>& key_table()
{
  static auto key_table = create_key_table();
  return key_table;
}

} // namespace

struct TCAttr {
 public:
  static bee::OrError<TCAttr> get_attr()
  {
    TCAttr attr;
    if (tcgetattr(STDIN_FILENO, &attr._attr) != 0) {
      return bee::Error::format(
        "Failed to get terminal attributes: $", strerror(errno));
    }
    return attr;
  }

  void make_raw()
  {
    cfmakeraw(&_attr);
    _attr.c_cc[VMIN] = 1;
    _attr.c_cc[VTIME] = 0;
  }

  bee::OrError<bee::Unit> set_attr() const
  {
    if (tcsetattr(STDIN_FILENO, TCSANOW, &_attr) != 0) {
      return bee::Error::format(
        "Failed to set terminal attributes: $", strerror(errno));
    }
    return bee::ok();
  }

 private:
  termios _attr;
};

////////////////////////////////////////////////////////////////////////////////
// Termino
//

Termino::Termino()
    : _stdout(bee::FileDescriptor::stdout_filedesc()),
      _stdin(bee::FileDescriptor::stdin_filedesc())
{
  // TODO: We should make sure ot get the window size at the beginning somehow
}

void Termino::_erase_screen() { _write_control_seq("2J"); }

void Termino::erase_screen()
{
  for (auto& l : _screen) { l.clear(); }
}
void Termino::hide_cursor() { _write_control_seq("?25l"); }
void Termino::show_cursor() { _write_control_seq("?25h"); }
void Termino::save_screen() { _write_control_seq("?47h"); }
void Termino::restore_screen() { _write_control_seq("?47l"); }
void Termino::save_cursor_position() { _write_control_seq("s"); }
void Termino::restore_cursor_position() { _write_control_seq("u"); }

bee::OrError<Size> Termino::_get_window_size()
{
  winsize ws;
  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) != 0) {
    return bee::Error::format("Failed to get window size: $", strerror(errno));
  }

  return Size{.height = ws.ws_row, .width = ws.ws_col};
}

const Size& Termino::get_window_size() const { return _window_size; }

void Termino::enable_alternative_buffer() { _write_control_seq("?1049h"); }

void Termino::disable_alternative_buffer() { _write_control_seq("?1049l"); }

void Termino::move_cursor_abs(int line, int col)
{
  _write_control_seq(format("$;$H", line + 1, col + 1));
}

void Termino::write_text_at(size_t line, size_t col, const vector<Cell>& text)
{
  if (_screen.size() <= line) { _screen.resize(line + 1); }
  vector<Cell>& l = _screen[line];
  if (l.size() < col + text.size()) {
    l.resize(col + text.size(), Cell::space());
  }

  for (size_t i = 0; i < text.size(); i++) { l[i + col] = text[i]; }
}

Termino::~Termino()
{
  if (_is_raw) { must_unit(restore_normal_mode()); }
}

bee::OrError<bee::Unit> Termino::make_raw_mode()
{
  if (_is_raw) return bee::Error("Already in raw mode");
  bail(attr, TCAttr::get_attr());
  _original_attr = make_unique<TCAttr>(attr);
  attr.make_raw();
  attr.set_attr();
  hide_cursor();
  save_screen();
  save_cursor_position();
  _erase_screen();
  enable_alternative_buffer();
  _is_raw = true;

  return flush_output();
}

bee::OrError<bee::Unit> Termino::restore_normal_mode()
{
  if (!_is_raw) { return bee::Error("Not in raw mode"); }

  disable_alternative_buffer();
  if (_original_attr != nullptr) {
    bail_unit(_original_attr->set_attr());
    _original_attr.reset();
  }
  restore_screen();
  show_cursor();
  restore_cursor_position();
  _is_raw = false;
  return flush_output();
}

void Termino::_write_cstring(const char* data)
{
  _buffer.write(reinterpret_cast<const std::byte*>(data), strlen(data));
}
void Termino::_write_char(char c) { _buffer.write(std::byte(c)); }

void Termino::_write_string(const string& data)
{
  _buffer.write(reinterpret_cast<const std::byte*>(data.data()), data.size());
}

void Termino::_write_escape(const char* data)
{
  _write_char(escape_char);
  _write_cstring(data);
}

void Termino::_write_control_seq(const char* data)
{
  _write_char(escape_char);
  _write_char(control_seq_char);
  _write_cstring(data);
}

void Termino::_write_control_seq(const string& data)
{
  _write_char(escape_char);
  _write_char(control_seq_char);
  _write_string(data);
}

bee::OrError<bee::Unit> Termino::flush_output()
{
  if (!_buffer.empty()) {
    size_t offset = 0;
    while (offset < _buffer.size()) {
      bail(
        bytes_read,
        _stdout->write(_buffer.raw_data() + offset, _buffer.size() - offset));
      offset += bytes_read;
    }
    _buffer.clear();
  }
  bail_assign(_window_size, _get_window_size());
  return bee::ok();
}

bee::OrError<bee::Unit> Termino::draw()
{
  int largest = std::max<int>(_screen.size(), _previous_screen.size());
  _screen.resize(largest);
  _previous_screen.resize(largest);

  int last_color = -2;
  int last_background = -2;

  for (int i = 0; i < _window_size.height; i++) {
    vector<Cell> new_line;
    if (i < int(_screen.size())) new_line = _screen[i];
    int min_size = new_line.size();
    bool dirty = false;

    if (i < int(_previous_screen.size())) {
      const vector<Cell>& prev_line = _previous_screen[i];
      if (prev_line != new_line) {
        min_size = std::max<int>(min_size, prev_line.size());
        dirty = true;
      }
    } else {
      dirty = !new_line.empty();
    }
    if (dirty) {
      min_size = std::min(min_size, _window_size.width);
      new_line.resize(min_size, Cell::space());
      move_cursor_abs(i, 0);
      for (const Cell& c : new_line) {
        if (last_color != c.foreground) {
          last_color = c.foreground;
          _set_color(last_color);
        }
        if (last_background != c.background) {
          last_background = c.background;
          _set_background(last_background);
        }
        _write_char(c.chr);
      }
      if (i < largest) { _previous_screen[i] = _screen[i]; }
    }
  }

  return flush_output();
}

bee::OrError<bee::Unit> Termino::set_blocking_input(bool blocking)
{
  return _stdin->set_blocking(blocking);
}

bee::OrError<optional<Key>> Termino::read_input()
{
  char buf[32];
  bail(ret, _stdin->read(reinterpret_cast<std::byte*>(buf), sizeof(buf)));
  if (ret.bytes_read() == 0) { return nullopt; }

  string raw(buf, buf + ret.bytes_read());

  auto& table = key_table();
  auto it = table.find(raw);
  if (it == table.end()) {
    return Key{
      .character = 0,
      .key_code = KeyCode::Unknown,
      .ctrl_pressed = false,
      .shift_pressed = false,
      .raw = raw,
      .name = "Unknown"};
  }

  return it->second;
}

void Termino::_set_color(int color)
{
  if (color == -1) {
    _write_control_seq(format("39m"));
  } else {
    _write_control_seq(format("38;5;$m", color));
  }
}

void Termino::_set_background(int color)
{
  if (color == -1) {
    _write_control_seq(format("49m"));
  } else {
    _write_control_seq(format("48;5;$m", color));
  }
}

const bee::FileDescriptor::shared_ptr& Termino::input_fd() const
{
  return _stdin;
}

} // namespace termino
