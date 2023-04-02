#include "termino_app.hpp"

#include "async/deferred_awaitable.hpp"
#include "async/scheduler_context.hpp"

using namespace async;
using std::weak_ptr;

namespace termino {

void TerminoApp::_tear_down(bee::OrError<bee::Unit>&& error)
{
  if (_closing) return;
  _closing = true;
  must_unit(_termino.restore_normal_mode());
  async::schedule_task(
    [ptr = shared_from_this(), error = std::move(error)]() -> Task<bee::Unit> {
      co_await ptr->tear_down();
      ptr->_exit_ivar->resolve(error);
      co_return bee::unit;
    });
}

void TerminoApp::tear_down_if_error(bee::OrError<bee::Unit>&& error)
{
  if (error.is_error()) { _tear_down(std::move(error)); }
}

Task<bee::OrError<bee::Unit>> TerminoApp::start()
{
  auto ptr = this->shared_from_this();
  weak_ptr<TerminoApp> weak_app(ptr);

  co_bail_unit(SchedulerContext::scheduler().add_fd(
    ptr->_termino.input_fd(), [weak_app]() {
      auto ptr = weak_app.lock();
      if (ptr == nullptr) { return; }
      ptr->_handle_input();
    }));

  co_bail_unit(_termino.make_raw_mode());
  co_bail_unit(_termino.set_blocking_input(false));

  co_bail_unit(refresh());

  co_return co_await _exit_ivar;
}

bool TerminoApp::is_done() const { return _done; }

void TerminoApp::mark_done() { _done = true; }

TerminoApp::~TerminoApp() {}

TerminoApp::TerminoApp()
    : _exit_ivar(async::Ivar<bee::OrError<bee::Unit>>::create()),
      _done(false),
      _closing(false)
{}

bee::OrError<bee::Unit> TerminoApp::refresh()
{
  bail_unit(render());
  _termino.erase_screen();
  if (_root != nullptr) {
    auto available_space = _termino.get_window_size();
    _root->reflow(available_space);
    _root->draw(_termino, 0, 0);
  }
  return _termino.draw();
}

void TerminoApp::_handle_input()
{
  tear_down_if_error([&]() -> bee::OrError<bee::Unit> {
    while (!is_done()) {
      bail(input, _termino.read_input());
      if (!input.has_value()) { break; }
      bail_unit(handle_input(input.value()));
    }
    if (is_done()) { _tear_down(bee::ok()); }
    return refresh();
    ;
  }());
}

void TerminoApp::set_root(const termino::Element::ptr& root) { _root = root; }

} // namespace termino
