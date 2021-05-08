#ifndef DECLARATIONS_H
#define DECLARATIONS_H

enum VariableType { INT, STR };

enum StatementType {
  REM,
  LET,
  PRINT,
  PRINTF,
  INPUT,
  INPUTS,
  GOTO,
  IF,
  END,
  ERR
};

class BasicInterpreter;

class Environment;

class Statement;

class Environment;

class Expression;

#endif  // DECLARATIONS_H
