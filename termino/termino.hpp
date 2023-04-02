#pragma once

#include "cell.hpp"
#include "key.hpp"
#include "key_code.hpp"
#include "size.hpp"

#include "bee/bytes_buffer.hpp"
#include "bee/error.hpp"
#include "bee/file_descriptor.hpp"

#include <optional>

namespace termino {

struct TCAttr;

struct Termino {
 public:
  Termino();
  ~Termino();

  void erase_screen();
  void hide_cursor();
  void show_cursor();
  void save_screen();
  void restore_screen();
  void save_cursor_position();
  void restore_cursor_position();

  const Size& get_window_size() const;

  void enable_alternative_buffer();
  void disable_alternative_buffer();

  void move_cursor_abs(int line, int col);

  void write_text_at(size_t line, size_t col, const std::vector<Cell>& text);

  bee::OrError<bee::Unit> make_raw_mode();

  bee::OrError<bee::Unit> restore_normal_mode();

  bee::OrError<std::optional<Key>> read_input();
  bee::OrError<std::optional<Key>> read_input_nonblock();

  bee::OrError<bee::Unit> flush_output();

  bee::OrError<bee::Unit> draw();

  const bee::FileDescriptor::shared_ptr& input_fd() const;

  bee::OrError<bee::Unit> set_blocking_input(bool blocking);

 private:
  void _write_char(char c);
  void _write_cstring(const char* data);
  void _write_string(const std::string& data);
  void _write_escape(const char* data);
  void _write_control_seq(const char* data);
  void _write_control_seq(const std::string& data);
  bee::OrError<Size> _get_window_size();

  void _erase_screen();
  void _set_color(int color);
  void _set_background(int color);

  bool _is_raw = false;
  std::unique_ptr<TCAttr> _original_attr;
  bee::FileDescriptor::shared_ptr _stdout;
  bee::FileDescriptor::shared_ptr _stdin;

  bee::BytesBuffer _buffer;

  std::vector<std::vector<Cell>> _screen;
  std::vector<std::vector<Cell>> _previous_screen;

  Size _window_size;
};

} // namespace termino
