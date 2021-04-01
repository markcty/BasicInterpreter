#include "mainwindow.h"

#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  connect(ui->loadButton, &QPushButton::clicked, this, &MainWindow::load);
  connect(ui->excuteButton, &QPushButton::clicked, this, &MainWindow::execute);
  connect(ui->clearButton, &QPushButton::clicked, this, &MainWindow::clear);
  connect(ui->loadAction, &QAction::triggered, this, &MainWindow::load);
  connect(ui->runAction, &QAction::triggered, this, &MainWindow::execute);
  connect(ui->clearAction, &QAction::triggered, this, &MainWindow::clear);
  connect(ui->cmd, &QLineEdit::returnPressed, this, &MainWindow::getCMD);

  ui->output->setReadOnly(true);
  ui->output->installEventFilter(this);

  newInterpreter();
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event) {
  if (watched == ui->output) {
    if (event->type() == QKeyEvent::KeyPress) {
      QKeyEvent *key = static_cast<QKeyEvent *>(event);
      if (key->key() == Qt::Key_Return || key->key() == Qt::Key_Enter) {
        ui->output->setReadOnly(true);
        QString input = ui->output->toPlainText();
        input = input.mid(input.lastIndexOf('?') + 1);
        interpreter->setInput(input.toInt());
        ui->cmd->setFocus();
        return true;  // do not process this event further
      }
    }
    return false;  // process this event further
  } else {
    // pass the event on to the parent class
    return QMainWindow::eventFilter(watched, event);
  }
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::newInterpreter() {
  interpreter = new BasicInterpreter();
  connect(interpreter, &BasicInterpreter::needInput, this,
          &MainWindow::getInput);
  connect(interpreter, &BasicInterpreter::needOutput, this, &MainWindow::print);
  connect(interpreter, &BasicInterpreter::needLoad, this, &MainWindow::load);
  connect(interpreter, &BasicInterpreter::needPrintExpTree, this,
          &MainWindow::printExpTree);
  connect(interpreter, &BasicInterpreter::needClearScreen, this,
          &MainWindow::clearScreen);
}

void MainWindow::load() {
  QString fileName = QFileDialog::getOpenFileName(this, "Open the file");
  QFile file(fileName);
  if (!file.open(QIODevice::ReadOnly | QFile::Text)) {
    QMessageBox::warning(this, "Warning",
                         "Cannot open file: " + file.errorString());
    return;
  }
  setWindowTitle(fileName);
  QTextStream in(&file);

  if (interpreter) delete interpreter;
  newInterpreter();
  clearScreen();
  try {
    while (!in.atEnd()) {
      interpreter->parseCmd(in.readLine());
    }
  } catch (const QStringException &e) {
    QMessageBox::warning(this, "Error", e.what());
  }

  file.close();
  ui->source->setText(interpreter->toString());
}

void MainWindow::clear() {
  try {
    interpreter->parseCmd("CLEAR");
  } catch (const QStringException &e) {
    QMessageBox::warning(this, "Error", e.what());
  }
}

void MainWindow::clearScreen() {
  ui->cmd->clear();
  ui->output->clear();
  ui->expressionTree->clear();
  ui->source->setText(interpreter->toString());
}

void MainWindow::execute() {
  try {
    interpreter->parseCmd("RUN");
  } catch (const QStringException &e) {
    QMessageBox::warning(this, "Error", e.what());
  }
}

void MainWindow::getCMD() {
  try {
    interpreter->parseCmd(ui->cmd->text().simplified());
  } catch (const QStringException &e) {
    QMessageBox::warning(this, "Error", e.what());
  }
  ui->cmd->clear();
  ui->source->setText(interpreter->toString());
}

void MainWindow::getInput() {
  ui->output->append("        ?  ");
  ui->output->setReadOnly(false);
  ui->output->setFocus();
}

void MainWindow::print(QString output) { ui->output->append(output); }

void MainWindow::printExpTree(QString output) {
  ui->expressionTree->clear();
  ui->expressionTree->setText(output);
}
