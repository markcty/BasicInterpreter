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
  connect(ui->cmd, &QLineEdit::returnPressed, this, &MainWindow::parseCMD);

  ui->output->setReadOnly(true);
  ui->output->installEventFilter(this);

  interpreter = new BasicInterpreter();
  connect(interpreter, &BasicInterpreter::needInput, this,
          &MainWindow::getValue);
  connect(interpreter, &BasicInterpreter::print, this, &MainWindow::print);
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

void MainWindow::load() {
  qDebug() << "load!";
  QString fileName = QFileDialog::getOpenFileName(this, "Open the file");
  QFile file(fileName);
  if (!file.open(QIODevice::ReadOnly | QFile::Text)) {
    QMessageBox::warning(this, "Warning",
                         "Cannot open file: " + file.errorString());
    return;
  }
  setWindowTitle(fileName);
  QTextStream in(&file);

  while (!in.atEnd()) {
    interpreter->parseCmd(in.readLine());
  }

  file.close();
  ui->source->setText(interpreter->toString());
}

void MainWindow::clear() {
  qDebug("clear!");
  ui->source->clear();
  interpreter->parseCmd("CLEAR");
}

void MainWindow::execute() {
  qDebug("run!");
  interpreter->parseCmd("RUN");
}

void MainWindow::parseCMD() {
  qDebug() << "cmd!";
  interpreter->parseCmd(ui->cmd->text().simplified());
  ui->cmd->clear();
  ui->source->setText(interpreter->toString());
}

void MainWindow::getValue() {
  ui->output->append("        ?  ");
  ui->output->setReadOnly(false);
  ui->output->setFocus();
}

void MainWindow::print(QString output) { ui->output->append(output); }
