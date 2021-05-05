#ifndef BASICINTERPRETER_H
#define BASICINTERPRETER_H

#include <QApplication>
#include <QColor>
#include <QDebug>
#include <QMap>
#include <QObject>
#include <QString>
#include <QStringList>

#include "environment.h"
#include "expression.h"
#include "statement.h"

/*
 * Class: BasicInterpreter
 * -----------------------
 * This class implements a Basic interpreter.
 * It stores the program in a QMap.
 * It interacts with the user interface(can be either GUI or cmd)
 * by sending signals(need...).
 */
class BasicInterpreter : public QObject {
  Q_OBJECT
 private:
  // current execute mode is immediate(input without a line number)
  // or Run
  // or Debug
  // or Normal
  enum Mode { Immediate, Run, Debug, Normal } mode;

  // sources
  QMap<int, Statement *> src;

  // runtime environment
  Environment *env;

  // current immediate statement
  Statement *immediateStatement;

  // insert a line
  void insertLine(int index, QString line);

  // remove a line
  void removeLine(int index);

  // run the program
  void run();

  // get the line offset of statement with key as its line number
  int getLineOffset(int key) const;

  // parse sources, return true if no error, false otherwise
  bool parseSrc();

 public:
  BasicInterpreter();

  // The only interface: parse a command and call corresponding function
  void parseCmd(QString cmd);

  // return source
  QString getSource() const;

 signals:
  // require a step
  void nextStep();

  // require a input(INPUT statement)
  void needInput();

  // require a load from file
  void needLoad();

  // require printing
  void needOutput(QString output);

  // require printing the exp tree
  void needPrintExpTree(QString tree);

  // require a clear of the screen
  void needClearScreen();

  // require printing environment
  void needPrintEnv(QString output);

  // require output error
  void needErrorOutput(QString err);

  // require highlight lines
  void needHighlight(QList<QPair<int, QColor>> lines);

 public slots:
  // excute a statement
  void step();

  // set the variable(can only be used in INPUT statement)
  void setInput(int v);

  // start debug or step
  void debug();
};

#endif  // BASICINTERPRETER_H
