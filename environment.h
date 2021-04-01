#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <QMap>
#include <QString>

#include "basicinterpreter.h"
#include "expression.h"
#include "statement.h"

class Statement;

/*
 * Class: Environment
 * ------------------
 * This subclass stores runtime environment of a program
 * (current executing line, variable values)
 */

class Environment {
 private:
  QMap<QString, int> env;

 public:
  Environment() = default;

  QMap<int, Statement *>::const_iterator currentLine;

  // return the value of a variable
  int getValue(const QString &variable) const;

  // set the value of a variable
  void setValue(const QString &variable, int value);

  // clear the environment
  void clear();
};

#endif  // ENVIRONMENT_H
