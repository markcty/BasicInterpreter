#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDebug>
#include <QFileDialog>
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

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

 private:
  Ui::MainWindow *ui;
  BasicInterpreter *interpreter;

 public slots:
  void load();
  void clear();
  void execute();
  void parseCMD();
};
#endif  // MAINWINDOW_H
