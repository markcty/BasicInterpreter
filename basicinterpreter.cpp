#include "basicinterpreter.h"

void BasicInterpreter::parseCmd(QString cmd) {
  if (cmd.isEmpty()) return;
  auto parts = cmd.split(" ", Qt::SkipEmptyParts);
  bool isLineNumber = false;
  int index = parts[0].toInt(&isLineNumber);
  try {
    // part of the program
    if (isLineNumber) {
      if (parts.length() > 1)
        insertLine(
            index,
            cmd.remove(cmd.indexOf(parts[0]), parts[0].length()).simplified());
      else
        removeLine(index);
      emit sourceChanged();
    }
    // immediate statement
    else if (parts[0] == "PRINT") {
      setMode(Immediate);
      immediateStatement = new PrintStatement(cmd);
      immediateStatement->parse();
      emit needOutput(
          QString::number(immediateStatement->getFirstExp()->eval(*env)));
      delete immediateStatement;
      setMode(Normal);
    } else if (parts[0] == "LET") {
      setMode(Immediate);
      auto let = new LetStatement(cmd);
      let->parse();
      if (let->getType() == STR)
        env->setValue(let->getVariable(), let->getVal());
      else
        env->setValue(let->getVariable(), let->getFirstExp()->eval(*env));
      emit needPrintEnv(env->toString());
      delete let;
      setMode(Normal);
    } else if (parts[0] == "INPUT") {
      setMode(Immediate);
      immediateStatement = new InputStatement(cmd);
      immediateStatement->parse();
      emit needInput();
    } else if (parts[0] == "INPUTS") {
      setMode(Immediate);
      immediateStatement = new InputsStatement(cmd);
      immediateStatement->parse();
      emit needInput();
    } else if (parts[0] == "PRINTF") {
      setMode(Immediate);
      PrintfStatement *s = new PrintfStatement(cmd);
      s->parse();
      emit needOutput(s->compose(*env));
      setMode(Normal);
    }
    // command
    else if (parts[0] == "RUN") {
      emit needClearScreen();
      run();
    } else if (parts[0] == "LOAD") {
      emit needLoad();
    } else if (parts[0] == "LIST") {
      // no need to implement
    } else if (parts[0] == "CLEAR") {
      src.clear();
      env->clear();
      errLines.clear();
      emit needClearScreen();
    } else if (parts[0] == "QUIT") {
      QApplication::quit();
    } else if (parts[0] == "HELP") {
      emit needOutput(
          "Basic Interpreter implemented by markcty. Please read Basic-doc "
          "before use");
    } else
      throw QStringException("Invalid Command!");
  } catch (const QStringException &e) {
    setMode(Normal);
    emit needPopUp(e.what());
  }
}

void BasicInterpreter::insertLine(int index, QString line) {
  Statement *statement = nullptr;
  auto parts = line.split(" ");
  QString type = parts[0].simplified();
  if (type == "REM")
    statement = new RemStatement(line);
  else if (type == "LET")
    statement = new LetStatement(line);
  else if (type == "PRINT")
    statement = new PrintStatement(line);
  else if (type == "INPUT")
    statement = new InputStatement(line);
  else if (type == "INPUTS")
    statement = new InputsStatement(line);
  else if (type == "GOTO")
    statement = new GotoStatement(line);
  else if (type == "IF")
    statement = new IfStatement(line);
  else if (type == "END")
    statement = new EndStatement(line);
  else if (type == "PRINTF")
    statement = new PrintfStatement(line);
  else
    statement = new InvalidStatement(line);
  src[index] = statement;
}

void BasicInterpreter::removeLine(int index) { src.remove(index); }

QString BasicInterpreter::getSource() const {
  QStringList lines;
  QMapIterator<int, Statement *> i(src);
  while (i.hasNext()) {
    i.next();
    lines.append(QString::number(i.key()) + " " + i.value()->toString());
  }
  return lines.join("\n");
}

BasicInterpreter::BasicInterpreter(QObject *parent)
    : QObject(parent), env(new Environment) {
  connect(this, &BasicInterpreter::nextStep, this, &BasicInterpreter::step);
  setMode(Normal);
}

void BasicInterpreter::step() {
  if (env->currentLine == src.constEnd()) {
    setMode(Normal);
    env->clear();
    throw QStringException("The program ends without an END statement");
  }

  if (env->currentLine.value()->statementType == ERR) {
    setMode(Normal);
    env->clear();
    throw QStringException("The program ends because of a corrupted statement");
  }

  auto shouldStep = [&]() {
    if (getMode() == Run)
      emit nextStep();
    else {
      QList<QPair<int, QColor>> lines;
      if (env->currentLine != src.constEnd()) {
        int offset = getLineOffset(env->currentLine.key());
        lines.append(QPair<int, QColor>{offset, QColor(124, 252, 0)});
        emit needPrintExpTree(env->currentLine.value()->toTree());
      }
      lines.append(errLines);
      emit needHighlight(lines);
    }
  };

  Statement *statement = env->currentLine.value();
  switch (statement->statementType) {
    case LET: {
      auto *let = dynamic_cast<LetStatement *>(statement);
      if (let->getType() == STR)
        env->setValue(let->getVariable(), let->getVal());
      else
        env->setValue(statement->getVariable(),
                      statement->getFirstExp()->eval(*env));
      env->currentLine++;
      emit needPrintEnv(env->toString());
      shouldStep();
      break;
    }
    case GOTO: {
      env->currentLine = src.constFind(statement->getLineNumber());
      if (env->currentLine == src.constEnd())
        throw QStringException("Line " +
                               QString::number(statement->getLineNumber()) +
                               " does not exist");
      shouldStep();
      break;
    }
    case IF: {
      QString op = statement->getOperator();
      int lv = statement->getFirstExp()->eval(*env),
          rv = statement->getSecondExp()->eval(*env);
      if ((op == ">" && lv > rv) || (op == "=" && lv == rv) ||
          (op == "<" && lv < rv)) {
        env->currentLine = src.constFind(statement->getLineNumber());
        if (env->currentLine == src.constEnd())
          throw QStringException("Line " +
                                 QString::number(statement->getLineNumber()) +
                                 " does not exist");
      } else
        env->currentLine++;
      shouldStep();
      break;
    }
    // the input statement is a special case because it requires
    // asynchronous treatment
    case INPUT: {
      emit needInput();
      break;
    }
    case INPUTS: {
      emit needInput();
      break;
    }
    case PRINT: {
      emit needOutput(QString::number(statement->getFirstExp()->eval(*env)));
      env->currentLine++;
      shouldStep();
      break;
    }
    case PRINTF: {
      auto st = dynamic_cast<PrintfStatement *>(statement);
      emit needOutput(st->compose(*env));
      env->currentLine++;
      shouldStep();
      break;
    }
    case END: {
      env->currentLine = src.constEnd();
      setMode(Normal);
      QList<QPair<int, QColor>> lines;
      lines.append(errLines);
      emit needHighlight(lines);
      emit needPrintEnv(env->toString());
      emit needPopUp("The program ends normally");
      env->clear();
      break;
    }
    default:
      env->currentLine++;
      shouldStep();
      break;
  }
}

void BasicInterpreter::setInput(QString input) {
  auto currentStatement =
      getMode() == Immediate ? immediateStatement : env->currentLine.value();

  if (currentStatement->statementType == INPUT) {
    bool ok;
    env->setValue(currentStatement->getVariable(), input.toInt(&ok));
    if (!ok) {
      emit needPopUp("Input invalid");
      emit needInput();
      return;
    }
  } else
    env->setValue(currentStatement->getVariable(), input);

  emit needPrintEnv(env->toString());

  if (getMode() == Immediate) {
    delete immediateStatement;
    setMode(Normal);
  } else {
    env->currentLine++;
    if (getMode() == Run) try {
        emit nextStep();
      } catch (QStringException &e) {
        setMode(Normal);
        emit needPopUp(e.what());
      }
    else {
      QList<QPair<int, QColor>> lines;
      int offset = getLineOffset(env->currentLine.key());
      lines.append(QPair<int, QColor>{offset, QColor(124, 252, 0)});
      lines.append(errLines);
      emit needHighlight(lines);
    }
  }
}

void BasicInterpreter::debug() {
  try {
    if (getMode() != Debug && !src.empty()) {
      emit needClearScreen();
      parseSrc();
      setMode(Debug);
      env->currentLine = src.constBegin();
      QList<QPair<int, QColor>> lines;
      int offset = getLineOffset(env->currentLine.key());
      lines.append(QPair<int, QColor>{offset, QColor(124, 252, 0)});
      lines.append(errLines);
      emit needHighlight(lines);
      emit needPrintExpTree(env->currentLine.value()->toTree());
    } else
      emit nextStep();
  } catch (const QStringException &err) {
    emit needPopUp(err.what());
    setMode(Normal);
    QList<QPair<int, QColor>> lines;
    lines.append(errLines);
    emit needHighlight(lines);
  }
}

void BasicInterpreter::setMode(Mode m) {
  m_mode = m;
  emit modeChanged(m_mode);
}

BasicInterpreter::Mode BasicInterpreter::getMode() { return m_mode; }

void BasicInterpreter::run() {
  if (src.empty()) return;

  auto prev_mode = getMode();
  setMode(Run);
  if (prev_mode != Debug) {
    env->currentLine = src.constBegin();
    parseSrc();
  }

  QString tree;
  for (auto i = src.constBegin(); i != src.constEnd(); i++)
    tree.push_back(QString::number(i.key()) + " " + i.value()->toTree());
  emit needPrintExpTree(tree);
  emit nextStep();
}

int BasicInterpreter::getLineOffset(int key) const {
  auto keys = src.keys();
  return keys.indexOf(key);
}

void BasicInterpreter::parseSrc() {
  QMapIterator<int, Statement *> line(src);
  int i = 0;
  errLines.clear();
  while (line.hasNext()) {
    line.next();
    try {
      line.value()->parse();
    } catch (const QStringException &e) {
      line.value()->statementType = ERR;
      QPair<int, QColor> pair{i, QColor(255, 0, 0)};
      errLines.append(pair);
    }
    i++;
  }
  if (!errLines.empty()) emit needHighlight(errLines);
}
