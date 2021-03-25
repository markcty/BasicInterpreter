#include "basicinterpreter.h"

bool BasicInterpreter::parseCmd(QString cmd) {
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
    return true;
  }
  // immdiate statement
  else if (parts[0] == "PRINT") {
    immediateStatement = new PrintStatement(cmd);
    emit needPrint(
        QString::number(immediateStatement->getFirstExp()->eval(*env)));
  } else if (parts[0] == "LET") {
    immediateStatement = new LetStatement(cmd);
    env->setValue(immediateStatement->getVariable(),
                  immediateStatement->getFirstExp()->eval(*env));
  } else if (parts[0] == "INPUT") {
    immediateStatement = new InputStatement(cmd);
    emit needInput();
  }
  // command
  else if (parts[0] == "RUN") {
    run();
  } else if (parts[0] == "LOAD") {
    emit needLoad();
  } else if (parts[0] == "LIST") {
    // no need to implement
  } else if (parts[0] == "CLEAR") {
    src.clear();
  } else
    return false;
  return true;
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
    return;
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
      emit nextStep();
      break;
    }
    case IF: {
      QChar op = statement->getOperator();
      int lv = statement->getFirstExp()->eval(*env),
          rv = statement->getSecondExp()->eval(*env);
      if ((op == '>' && lv > rv) || (op == '=' && lv == rv) ||
          (op == '<' && lv < rv))
        env->currentLine = src.constFind(statement->getLineNumber());
      else
        env->currentLine++;
      emit nextStep();
      break;
    }
    case INPUT: {
      emit needInput();
      break;
    }
    case PRINT: {
      emit needPrint(QString::number(statement->getFirstExp()->eval(*env)));
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
  env->clear();
  emit needPrint("--------------");
  emit nextStep();
}
