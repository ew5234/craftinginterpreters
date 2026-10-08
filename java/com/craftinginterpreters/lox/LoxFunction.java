//> Functions lox-function
package com.craftinginterpreters.lox;

import java.util.List;

class LoxFunction implements LoxCallable {
  private final String name;
  private final Expr.Function declaration;
//> closure-field
  private final Environment closure;
  
//< closure-field
/* Functions lox-function < Functions closure-constructor
  LoxFunction(Stmt.Function declaration) {
*/
/* Functions closure-constructor < Classes is-initializer-field
  LoxFunction(Stmt.Function declaration, Environment closure) {
*/
//> Classes is-initializer-field
  private final boolean isInitializer;
  private LoxClass declaringClass;
  private final LoxInstance boundInstance;
  LoxFunction(String name, Expr.Function declaration, Environment closure,
            boolean isInitializer) {
    this(name, declaration, closure, isInitializer, null);
  }

  private LoxFunction(String name, Expr.Function declaration, Environment closure,
                      boolean isInitializer, LoxInstance boundInstance) {
    this.name = name;
    this.isInitializer = isInitializer;
    this.closure = closure;
    this.declaration = declaration;
    this.boundInstance = boundInstance;
  }

//> Classes bind-instance
  LoxFunction bind(LoxInstance instance) {
    Environment environment = new Environment(closure);
    environment.defineSlot(instance);

    LoxFunction function = new LoxFunction(
        name,
        declaration,
        environment,
        isInitializer,
        instance
    );

    function.declaringClass = declaringClass;
    return function;
  }
//< Classes bind-instance
//> function-to-string
  @Override
  public String toString() {
    if (name == null) return "<fn>";
    return "<fn " + name + ">";
  }
//< function-to-string
//> function-arity
  @Override
  public int arity() {
    return declaration.parameters.size();
  }
//< function-arity
//> function-call
  @Override
  public Object call(Interpreter interpreter,
                    List<Object> arguments) {
    Environment environment = new Environment(closure);

    if (declaration.parameters != null) {
      for (int i = 0; i < declaration.parameters.size(); i++) {
        environment.defineSlot(arguments.get(i));
      }
    }

    LoxFunction previousFunction = interpreter.currentFunction;
    interpreter.currentFunction = this;

    try {
      interpreter.executeBlock(declaration.body, environment);
    } catch (Return returnValue) {
      if (isInitializer) return closure.getAt(0, 0);
      return returnValue.value;
    } finally {
      interpreter.currentFunction = previousFunction;
    }

    if (isInitializer) return closure.getAt(0, 0);
    return null;
  }
//< function-call

  public boolean isGetter() {
    return declaration.parameters == null;
  }

  void setDeclaringClass(LoxClass klass) {
    this.declaringClass = klass;
  }

  LoxClass getDeclaringClass(){
    return declaringClass;
  }

  LoxInstance getBoundInstance() {
    return boundInstance;
  }

  String getName() {
    return name;
  }
}
