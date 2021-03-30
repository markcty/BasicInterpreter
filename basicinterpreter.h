#ifndef BASICINTERPRETER_H
#define BASICINTERPRETER_H

#include <QDebug>
#include <QMap>
#include <QObject>
#include <QString>
#include <QStringList>

#include "environment.h"
#include "expression.h"
#include "statement.h"

class BasicInterpreter : public QObject {
  Q_OBJECT
 private:
  enum Mode { Immediate, Continuous } mode;

  QMap<int, Statement *> src;
  Environment *env;

  Statement *immediateStatement;

  void insertLine(int index, QString line);
  void removeLine(int index);
  void run();

 public:
  void parseCmd(QString cmd);
  QString toString() const;
  BasicInterpreter();

 signals:
  void nextStep();
  void needInput();
  void needLoad();
  void needOutput(QString output);
  void needPrintExpTree(QString tree);
  void clearScreen();

 public slots:
  void step();
  void setInput(int v);
};

#endif  // BASICINTERPRETER_H
