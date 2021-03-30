#include "basicinterpreter.h"

void BasicInterpreter::parseCmd(QString cmd) {
  auto parts = cmd.split(" ", QString::SkipEmptyParts);
  bool isLineNumber = false;
  int index = parts[0].toInt(&isLineNumber);
  // part of a program
  if (isLineNumber) {
    if (parts.length() > 1)
      insertLine(
          index,
          cmd.remove(cmd.indexOf(parts[0]), parts[0].length()).simplified());
    else
      removeLine(index);
  }
  // immdiate statement
  else if (parts[0] == "PRINT") {
    mode = Immediate;
    immediateStatement = new PrintStatement(cmd);
    emit needOutput(
        QString::number(immediateStatement->getFirstExp()->eval(*env)));
    delete immediateStatement;
  } else if (parts[0] == "LET") {
    mode = Immediate;
    immediateStatement = new LetStatement(cmd);
    env->setValue(immediateStatement->getVariable(),
                  immediateStatement->getFirstExp()->eval(*env));
    delete immediateStatement;
  } else if (parts[0] == "INPUT") {
    mode = Immediate;
    immediateStatement = new InputStatement(cmd);
    emit needInput();
  }
  // command
  else if (parts[0] == "RUN") {
    emit clearScreen();
    run();
  } else if (parts[0] == "LOAD") {
    emit needLoad();
  } else if (parts[0] == "LIST") {
    // no need to implement
  } else if (parts[0] == "CLEAR") {
    src.clear();
    env->clear();
    emit clearScreen();
  } else
    throw QStringException("Invalid Command!");
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
    throw QStringException("Invalid Statement");
  src[index] = statement;
}

void BasicInterpreter::removeLine(int index) { src.remove(index); }

QString BasicInterpreter::toString() const {
  QStringList lines;
  QMapIterator<int, Statement *> i(src);
  while (i.hasNext()) {
    i.next();
    lines.append(QString::number(i.key()) + " " + i.value()->toString());
  }
  return lines.join("\n");
}

BasicInterpreter::BasicInterpreter() : mode(Immediate), env(new Environment) {
  connect(this, &BasicInterpreter::nextStep, this, &BasicInterpreter::step);
}

void BasicInterpreter::step() {
  if (env->currentLine == src.constEnd())
    throw QStringException("The program ends without an END statement");
  Statement *statement = env->currentLine.value();
  switch (statement->type) {
    case LET: {
      env->setValue(statement->getVariable(),
                    statement->getFirstExp()->eval(*env));
      env->currentLine++;
      emit nextStep();
      break;
    }
    case GOTO: {
      env->currentLine = src.constFind(statement->getLineNumber());
      if (env->currentLine == src.constEnd())
        throw QStringException("Line " +
                               QString::number(statement->getLineNumber()) +
                               " does not exist");
      emit nextStep();
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
      emit nextStep();
      break;
    }
    case INPUT: {
      // the input statement is a special case because it requires
      // asynchroronous treatment
      emit needInput();
      break;
    }
    case PRINT: {
      emit needOutput(QString::number(statement->getFirstExp()->eval(*env)));
      env->currentLine++;
      emit nextStep();
      break;
    }
    case END: {
      env->currentLine = src.constEnd();
      break;
    }
    default:
      env->currentLine++;
      emit nextStep();
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
    emit nextStep();
  }
}

void BasicInterpreter::run() {
  mode = Continuous;
  env->currentLine = src.constBegin();
  QString tree;
  for (auto i = src.constBegin(); i != src.constEnd(); i++)
    tree.push_back(QString::number(i.key()) + " " + i.value()->toTree());
  emit needPrintExpTree(tree);
  emit nextStep();
}
