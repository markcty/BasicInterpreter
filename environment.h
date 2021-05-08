#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <QMap>
#include <QString>

#include "basicinterpreter.h"
#include "declarations.h"
#include "expression.h"
#include "statement.h"

/*
 * Class: Environment
 * ------------------
 * This subclass stores runtime environment of a program
 * (current executing line, variable values)
 */
class Environment {
 private:
  struct BasicValue {
    int intVal;
    QString strVal;
    VariableType type;
  };

  QMap<QString, BasicValue> env;

 public:
  Environment() = default;
  QMap<int, Statement *>::const_iterator currentLine;

  // return the value of a variable
  int getIntValue(const QString &variable) const;
  QString getStrValue(const QString &variable) const;
  VariableType getType(const QString &variable) const;

  // set the value of a variable
  void setValue(const QString &variable, int value);
  void setValue(const QString &variable, const QString &value);

  // clear the environment
  void clear();

  // return string representation
  QString toString() const;
};

#endif  // ENVIRONMENT_H
