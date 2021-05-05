#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QColor>
#include <QDebug>
#include <QFileDialog>
#include <QKeyEvent>
#include <QMainWindow>
#include <QMessageBox>

#include "basicinterpreter.h"
#include "environment.h"
#include "expression.h"
#include "statement.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

/*
 * Class: MainWindow
 * -----------------
 * This class is the GUI interface of Basic Interpreter
 */

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  MainWindow(QWidget *parent = nullptr);
  // process enter key in output
  bool eventFilter(QObject *watched, QEvent *event);
  ~MainWindow();

 private:
  Ui::MainWindow *ui;
  BasicInterpreter *interpreter;

  // wrapper for create a new interpreter and connect all signals
  void newInterpreter();

  // highlight lines
  void HightLines(QList<QPair<int, QColor>> lines);

 public slots:
  // handler for load button
  void load();

  // handler for clear button
  void clear();

  // handler for run button
  void execute();

  // forward command to basic interpreter
  void getCMD();

  // get a input from output window
  void getInput();

  // print output to output window
  void print(QString output);

  // print a exp tree to expression tree window
  void printExpTree(QString output);

  // clear screen
  void clearScreen();

  // notify error
  void notifyError(QString err);
};
#endif  // MAINWINDOW_H
