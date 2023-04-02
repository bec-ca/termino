#pragma once

#include "async/async.hpp"
#include "async/task.hpp"
#include "element.hpp"
#include "termino.hpp"

#include <memory>

namespace termino {

struct TerminoApp : public std::enable_shared_from_this<TerminoApp> {
 public:
  void tear_down_if_error(bee::OrError<bee::Unit>&& error);

  async::Task<bee::OrError<bee::Unit>> start();

  bool is_done() const;

  void mark_done();

  virtual ~TerminoApp();

 protected:
  void set_root(const termino::Element::ptr& root);
  TerminoApp();

  bee::OrError<bee::Unit> refresh();

  virtual bee::OrError<bee::Unit> render() = 0;

  virtual bee::OrError<bee::Unit> handle_input(const termino::Key& key) = 0;

  virtual async::Task<bee::Unit> tear_down() = 0;

 private:
  virtual void _handle_input();
  virtual void _tear_down(bee::OrError<bee::Unit>&& error);

  termino::Termino _termino;
  termino::Element::ptr _root;
  async::Ivar<bee::OrError<bee::Unit>>::ptr _exit_ivar;
  bool _done;

  bool _closing;
};

} // namespace termino
