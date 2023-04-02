#include "key_predicate.hpp"

#include "key.hpp"

#include <functional>

using std::function;
using std::make_unique;
using std::unique_ptr;

namespace termino {

struct KeyPredicateImpl {
 public:
  using ptr = unique_ptr<KeyPredicateImpl>;

  template <class F>
  explicit KeyPredicateImpl(F&& pred) : _pred(std::forward<F>(pred))
  {}

  bool evaluate(const Key& key) const { return _pred(key); }

  ptr clone() const { return make_unique<KeyPredicateImpl>(_pred); }

  function<bool(const Key&)> pred() const { return _pred; }

 private:
  function<bool(const Key&)> _pred;
};

KeyPredicateImpl::ptr operator||(
  const KeyPredicateImpl::ptr& p1, const KeyPredicateImpl::ptr& p2)
{
  return make_unique<KeyPredicateImpl>(
    [p1 = p1->pred(), p2 = p2->pred()](const Key& key) {
      return p1(key) || p2(key);
    });
}

namespace {

KeyPredicate _is_key_code(KeyCode code)
{
  return KeyPredicate(make_unique<KeyPredicateImpl>(
    [code](const Key& key) { return key.key_code == code; }));
}

KeyPredicate _is_printable_char(int character)
{
  return KeyPredicate(
    make_unique<KeyPredicateImpl>([character](const Key& key) {
      return key.key_code == KeyCode::Printable && key.character == character;
    }));
}

} // namespace

////////////////////////////////////////////////////////////////////////////////
// KeyPredicate
//

KeyPredicate::KeyPredicate(unique_ptr<KeyPredicateImpl>&& pred)
    : _pred(std::move(pred))
{}

KeyPredicate::KeyPredicate(KeyPredicate&& other) : _pred(std::move(other._pred))
{}

KeyPredicate::KeyPredicate(const KeyPredicate& other)
    : _pred(other._pred->clone())
{}

KeyPredicate::~KeyPredicate() {}

bool KeyPredicate::operator()(const Key& key) const
{
  return _pred->evaluate(key);
}

KeyPredicate KeyPredicate::is_key_code(KeyCode code)
{
  return _is_key_code(code);
}

KeyPredicate KeyPredicate::is_printable_char(int character)
{
  return _is_printable_char(character);
}

KeyPredicate KeyPredicate::operator||(const KeyPredicate& other) const
{
  return KeyPredicate(_pred || other._pred);
}

} // namespace termino
