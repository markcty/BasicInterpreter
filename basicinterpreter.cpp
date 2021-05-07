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
    }
    // immediate statement
    else if (parts[0] == "PRINT") {
      mode = Immediate;
      immediateStatement = new PrintStatement(cmd);
      immediateStatement->parse();
      emit needOutput(
          QString::number(immediateStatement->getFirstExp()->eval(*env)));
      delete immediateStatement;
    } else if (parts[0] == "LET") {
      mode = Immediate;
      auto let = new LetStatement(cmd);
      let->parse();
      if (let->getType() == STR)
        env->setValue(let->getVariable(), let->getVal());
      else
        env->setValue(let->getVariable(), let->getFirstExp()->eval(*env));
      emit needPrintEnv(env->toString());
      delete let;
    } else if (parts[0] == "INPUT") {
      mode = Immediate;
      immediateStatement = new InputStatement(cmd);
      immediateStatement->parse();
      emit needInput();
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
    emit needErrorOutput(e.what());
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
  else if (type == "GOTO")
    statement = new GotoStatement(line);
  else if (type == "IF")
    statement = new IfStatement(line);
  else if (type == "END")
    statement = new EndStatement(line);
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

BasicInterpreter::BasicInterpreter() : mode(Normal), env(new Environment) {
  connect(this, &BasicInterpreter::nextStep, this, &BasicInterpreter::step);
}

void BasicInterpreter::step() {
  if (env->currentLine == src.constEnd()) {
    mode = Normal;
    throw QStringException("The program ends without an END statement");
  }

  if (env->currentLine.value()->statementType == ERR) {
    mode = Normal;
    throw QStringException("The program ends because of a corrupted statement");
  }

  auto shouldStep = [&]() {
    if (mode == Run)
      emit nextStep();
    else {
      QList<QPair<int, QColor>> lines;
      if (env->currentLine != src.constEnd()) {
        int offset = getLineOffset(env->currentLine.key());
        lines.append(QPair<int, QColor>{offset, QColor(124, 252, 0)});
        emit needPrintExpTree(env->currentLine.value()->toString());
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
      shouldStep();
      emit needPrintEnv(env->toString());
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
    case INPUT: {
      // the input statement is a special case because it requires
      // asynchronous treatment
      emit needInput();
      break;
    }
    case PRINT: {
      emit needOutput(QString::number(statement->getFirstExp()->eval(*env)));
      env->currentLine++;
      shouldStep();
      break;
    }
    case END: {
      env->currentLine = src.constEnd();
      mode = Normal;
      QList<QPair<int, QColor>> lines;
      lines.append(errLines);
      emit needHighlight(lines);
      break;
    }
    default:
      env->currentLine++;
      shouldStep();
      break;
  }
}

void BasicInterpreter::setInput(int v) {
  if (mode == Immediate) {
    env->setValue(immediateStatement->getVariable(), v);
    delete immediateStatement;
  } else {
    auto currentStatement = env->currentLine.value();
    env->setValue(currentStatement->getVariable(), v);
    env->currentLine++;
    if (mode == Run)
      emit nextStep();
    else {
      QList<QPair<int, QColor>> lines;
      int offset = getLineOffset(env->currentLine.key());
      lines.append(QPair<int, QColor>{offset, QColor(124, 252, 0)});
      lines.append(errLines);
      emit needHighlight(lines);
    }
  }
  emit needPrintEnv(env->toString());
}

void BasicInterpreter::debug() {
  try {
    if (mode != Debug) {
      parseSrc();
      mode = Debug;
      env->currentLine = src.constBegin();
      QList<QPair<int, QColor>> lines;
      int offset = getLineOffset(env->currentLine.key());
      lines.append(QPair<int, QColor>{offset, QColor(124, 252, 0)});
      lines.append(errLines);
      emit needHighlight(lines);
      emit needClearScreen();
    } else
      emit nextStep();
  } catch (const QStringException &err) {
    emit needErrorOutput(err.what());
    mode = Normal;
    QList<QPair<int, QColor>> lines;
    lines.append(errLines);
    emit needHighlight(lines);
  }
}

void BasicInterpreter::run() {
  // parse
  parseSrc();
  // run
  mode = Run;
  env->currentLine = src.constBegin();
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
