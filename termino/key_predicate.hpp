#pragma once

#include "key.hpp"

#include <memory>

namespace termino {

struct KeyPredicateImpl;

struct KeyPredicate {
 public:
  explicit KeyPredicate(std::unique_ptr<KeyPredicateImpl>&& pred);
  KeyPredicate(const KeyPredicate& other);
  KeyPredicate(KeyPredicate&& other);

  ~KeyPredicate();

  bool operator()(const Key& key) const;

  static KeyPredicate is_key_code(KeyCode code);

  static KeyPredicate is_printable_char(int character);

  KeyPredicate operator||(const KeyPredicate& other) const;

 private:
  std::unique_ptr<KeyPredicateImpl> _pred;
};

} // namespace termino
