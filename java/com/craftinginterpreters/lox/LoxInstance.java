//> Classes lox-instance
package com.craftinginterpreters.lox;

import java.util.HashMap;
import java.util.Map;
import java.util.ArrayList;

class LoxInstance {
  private final LoxClass klass;
//> lox-instance-fields
  private final Map<String, Object> fields = new HashMap<>();
//< lox-instance-fields

  LoxInstance(LoxClass klass) {
    this.klass = klass;
  }

//> lox-instance-get-property
  Object get(Token name, Interpreter interpreter) {
    if (fields.containsKey(name.lexeme)) {
      return fields.get(name.lexeme);
    }

    LoxFunction method = klass.findMethod(name.lexeme);

    if (method != null) {
      LoxFunction bound = method.bind(this);

      if (method.isGetter()) {
        return bound.call(interpreter, new ArrayList<>());
      }

      return bound;
    }

    throw new RuntimeError(
        name,
        "Undefined property '" + name.lexeme + "'.");
  }
//< lox-instance-get-property
//> lox-instance-set-property
  void set(Token name, Object value) {
    fields.put(name.lexeme, value);
  }
//< lox-instance-set-property
  @Override
  public String toString() {
    return klass.name + " instance";
  }
}
