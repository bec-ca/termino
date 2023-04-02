#include "key.hpp"

#include "bee/format.hpp"
#include "constants.hpp"

using bee::format;
using std::string;

namespace termino {
namespace {

string raw_repr(const string& str)
{
  string output;
  for (char c : str) {
    if (!output.empty()) output += ' ';
    output += format(int(c));
    if (c < 127 && c >= 32) {
      output += format("($)", c);
    } else if (c == escape_char) {
      output += format("(ESC)");
    }
  }
  return output;
}

} // namespace

string Key::repr() { return format("$[$]", name, raw_repr(raw)); }

} // namespace termino
