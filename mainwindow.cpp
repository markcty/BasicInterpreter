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
        input = input.mid(input.lastIndexOf('?') + 1).simplified();
        interpreter->setInput(input);
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
  connect(interpreter, &BasicInterpreter::needPopUp, this,
          &MainWindow::notifyError);
  connect(interpreter, &BasicInterpreter::needHighlight, this,
          &MainWindow::HightLines);
  connect(ui->debugButton, &QPushButton::clicked, interpreter,
          &BasicInterpreter::debug);
  connect(interpreter, &BasicInterpreter::needPrintEnv, this,
          &MainWindow::printEnv);
  connect(interpreter, &BasicInterpreter::modeChanged, this,
          &MainWindow::detectMode);
  connect(interpreter, &BasicInterpreter::sourceChanged, this,
          &MainWindow::updateSource);
}

void MainWindow::HightLines(QList<QPair<int, QColor>> lines) {
  // generate index for each line
  int j = 0;
  QString src = ui->source->toPlainText();
  QVector<int> index;
  index.push_back(0);
  while ((j = src.indexOf('\n', j)) != -1) index.push_back(++j);

  // hightlight
  QList<QTextEdit::ExtraSelection> extras;
  QTextCursor cursor(ui->source->document());
  cursor.setCharFormat(QTextCharFormat());
  for (auto &line : lines) {
    QTextEdit::ExtraSelection h;
    h.cursor = cursor;
    h.cursor.setPosition(index[line.first]);
    h.cursor.movePosition(QTextCursor::StartOfLine);
    h.cursor.movePosition(QTextCursor::EndOfLine);
    h.format.setProperty(QTextFormat::FullWidthSelection, true);
    h.format.setBackground(line.second);
    extras.append(h);
  }

  ui->source->setExtraSelections(extras);
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

  while (!in.atEnd()) interpreter->parseCmd(in.readLine());

  file.close();
}

void MainWindow::clear() { interpreter->parseCmd("CLEAR"); }

void MainWindow::clearScreen() {
  ui->cmd->clear();
  ui->output->clear();
  ui->expressionTree->clear();
  ui->environment->clear();
  ui->source->setText(interpreter->getSource());

  QList<QTextEdit::ExtraSelection> extras;
  ui->source->setExtraSelections(extras);
}

void MainWindow::notifyError(QString err) {
  QMessageBox::warning(this, "Error", err);
}

void MainWindow::detectMode(BasicInterpreter::Mode mode) {
  switch (mode) {
    case BasicInterpreter::Debug:
      ui->cmd->setDisabled(true);
      ui->clearButton->setDisabled(true);
      ui->loadButton->setDisabled(true);
      break;
    case BasicInterpreter::Immediate:
      ui->cmd->setDisabled(true);
      ui->debugButton->setDisabled(true);
      ui->excuteButton->setDisabled(true);
      ui->clearButton->setDisabled(true);
      ui->loadButton->setDisabled(true);
      break;
    case BasicInterpreter::Normal:
      ui->cmd->setDisabled(false);
      ui->debugButton->setDisabled(false);
      ui->excuteButton->setDisabled(false);
      ui->clearButton->setDisabled(false);
      ui->loadButton->setDisabled(false);
      break;
    case BasicInterpreter::Run:
      ui->cmd->setDisabled(true);
      ui->debugButton->setDisabled(true);
      ui->excuteButton->setDisabled(true);
      ui->clearButton->setDisabled(true);
      ui->loadButton->setDisabled(true);
      break;
  }
}

void MainWindow::updateSource() {
  ui->source->setText(interpreter->getSource());
}

void MainWindow::execute() { interpreter->parseCmd("RUN"); }

void MainWindow::getCMD() {
  interpreter->parseCmd(ui->cmd->text().simplified());
  ui->cmd->clear();
}

void MainWindow::getInput() {
  ui->output->append("        ?  ");
  ui->output->setReadOnly(false);
  ui->output->setFocus();
}

void MainWindow::print(QString output) { ui->output->append(output); }

void MainWindow::printExpTree(QString output) {
  ui->expressionTree->setText(output);
}

void MainWindow::printEnv(QString output) { ui->environment->setText(output); }
