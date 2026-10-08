//> Classes lox-class
package com.craftinginterpreters.lox;

import java.util.List;
import java.util.Map;
import java.util.ArrayList;

/* Classes lox-class < Classes lox-class-callable
class LoxClass {
*/
//> lox-class-callable
class LoxClass extends LoxInstance implements LoxCallable {
//< lox-class-callable
  final String name;
//> Inheritance lox-class-superclass-field
  final LoxClass superclass;
//< Inheritance lox-class-superclass-field
/* Classes lox-class < Classes lox-class-methods

  LoxClass(String name) {
    this.name = name;
  }
*/
//> lox-class-methods
  private final Map<String, LoxFunction> methods;

/* Classes lox-class-methods < Inheritance lox-class-constructor
  LoxClass(String name, Map<String, LoxFunction> methods) {
*/
//> Inheritance lox-class-constructor
  LoxClass(String name, LoxClass metaclass, LoxClass superclass, 
           Map<String, LoxFunction> methods) {
    super(metaclass);
//< Inheritance lox-class-constructor
    this.name = name;
    this.superclass = superclass;
    this.methods = methods;
  }
//< lox-class-methods
//> lox-class-find-method
  LoxFunction findMethod(String name) {
    if(superclass != null) {
      LoxFunction method = superclass.findMethod(name);
      if (method != null) {
        return method;
      }
    }
    if (methods.containsKey(name)) {
      return methods.get(name);
    }
    return null;
  }
//< lox-class-find-method

  @Override
  public String toString() {
    return name;
  }
//> lox-class-call-arity
  @Override
  public Object call(Interpreter interpreter,
                     List<Object> arguments) {
    LoxInstance instance = new LoxInstance(this);
//> lox-class-call-initializer
    LoxFunction initializer = findMethod("init");
    if (initializer != null) {
      initializer.bind(instance).call(interpreter, arguments);
    }

//< lox-class-call-initializer
    return instance;
  }

  @Override
  public int arity() {
/* Classes lox-class-call-arity < Classes lox-initializer-arity
    return 0;
*/
//> lox-initializer-arity
    LoxFunction initializer = findMethod("init");
    if (initializer == null) return 0;
    return initializer.arity();
//< lox-initializer-arity
  }
//< lox-class-call-arity

  Map<String, LoxFunction> getMethods() {
    return methods;
  }

  LoxFunction findInnerMethod(String name, LoxClass declaringClass) {
    if (this == declaringClass) {
      return null;
    }

    List<LoxClass> chain = new ArrayList<>();

    LoxClass current = this;

    while (current != null && current != declaringClass) {
      chain.add(current);
      current = current.superclass;
    }

    if (current != declaringClass) {
      return null;
    }

    for (int i = chain.size() - 1; i >= 0; i--) {
      LoxClass klass = chain.get(i);

      LoxFunction method = klass.methods.get(name);

      if (method != null) {
        return method;
      }
    }

    return null;
  }
}
