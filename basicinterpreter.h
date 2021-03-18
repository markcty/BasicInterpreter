#ifndef BASICINTERPRETER_H
#define BASICINTERPRETER_H

#include <QMap>
#include <QString>
#include <QStringList>

#include "environment.h"
#include "expression.h"
#include "statement.h"

class BasicInterpreter {
 private:
  QMap<int, Statement *> src;

  void insertLine(int index, QString line);
  void removeLine(int index);
  void run();

 public:
  bool parseCmd(QString cmd);
  QString toString() const;
  BasicInterpreter() = default;
};

#endif  // BASICINTERPRETER_H
