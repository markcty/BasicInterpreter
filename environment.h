#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <QMap>
#include <QString>

#include "basicinterpreter.h"
#include "expression.h"
#include "statement.h"

class Statement;

class Environment {
 private:
  QMap<QString, int> env;

 public:
  QMap<int, Statement *>::const_iterator currentLine;
  Environment() = default;  // TODO: take a src as input
  int getValue(const QString &variable) const;
  void setValue(const QString &variable, int value);
};

#endif  // ENVIRONMENT_H
