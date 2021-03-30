#include "statement.h"
RemStatement::RemStatement(const QString &l) {
  type = REM;
  line = l;
}

LetStatement::LetStatement(QString l) {
  line = l;
  l = l.remove("LET").simplified();
  if (l.indexOf("=") == -1) throw QStringException("Invalid Statement");
  auto part = l.split("=");
  if (part.size() != 2) throw QStringException("Invalid Statement");
  variable = part[0].simplified();
  exp = new Expression(part[1].simplified());
  type = LET;
}

QString LetStatement::getVariable() { return variable; }

Expression *LetStatement::getFirstExp() { return exp; }

PrintStatement::PrintStatement(QString l) {
  line = l;
  l = l.remove("PRINT").simplified();
  exp = new Expression(l);
  type = PRINT;
}

Expression *PrintStatement::getFirstExp() { return exp; }

InputStatement::InputStatement(QString l) {
  line = l;
  l = l.remove("INPUT").simplified();
  variable = l;
  type = INPUT;
}

QString InputStatement::getVariable() { return variable; }

GotoStatement::GotoStatement(QString l) {
  line = l;
  l = l.remove("GOTO").simplified();
  bool ok = false;
  lineNumber = l.toInt(&ok);
  if (!ok) throw QStringException("Invalid Statement");
  type = GOTO;
}

int GotoStatement::getLineNumber() { return lineNumber; }

IfStatement::IfStatement(QString l) {
  line = l;
  l = l.remove("IF").simplified();
  if (l.contains('='))
    op = '=';
  else if (l.contains('<'))
    op = '<';
  else if (l.contains('>'))
    op = '>';
  else
    throw QStringException("Invalid Statement");
  auto split_op = l.split(op);
  exp1 = new Expression(split_op[0].simplified());
  auto split_then = split_op[1].split("THEN");
  exp2 = new Expression(split_then[0].simplified());
  bool ok = false;
  lineNumber = split_then[1].toInt(&ok);
  if (!ok) throw QStringException("Invalid Statement");
  type = IF;
}

Expression *IfStatement::getFirstExp() { return exp1; }

Expression *IfStatement::getSecondExp() { return exp2; }

QString IfStatement::getOperator() { return op; }

int IfStatement::getLineNumber() { return lineNumber; }
EndStatement::EndStatement(const QString &l) {
  line = l;
  type = END;
}

QString Statement::toString() const { return line; }

QString Statement::getVariable() { return ""; }

int Statement::getConstant() { return 0; }

int Statement::getLineNumber() { return 0; }

QString Statement::getOperator() { return " "; }

Expression *Statement::getFirstExp() { return nullptr; }

Expression *Statement::getSecondExp() { return nullptr; }

QString RemStatement::toTree() {
  QString tree("REM\n");
  tree.append("    " + line.midRef(line.indexOf(" ") + 1) + "\n");
  return tree;
}

QString LetStatement::toTree() {
  QString tree("LET =\n");
  tree.append("    " + variable + '\n');
  tree.append(exp->toTree() + '\n');
  return tree;
}

LetStatement::~LetStatement() { delete exp; }

QString InputStatement::toTree() {
  QString tree("INPUT =\n");
  tree.append("    " + variable + '\n');
  return tree;
}

QString GotoStatement::toTree() {
  QString tree("GOTO\n");
  tree.append("    " + QString::number(lineNumber) + '\n');
  return tree;
}

QString IfStatement::toTree() {
  QString tree("IF THEN\n");
  tree.append("    " + op + '\n');
  tree.append(exp1->toTree() + '\n');
  tree.append(exp2->toTree() + '\n');
  tree.append("    " + QString::number(lineNumber) + '\n');
  return tree;
}

IfStatement::~IfStatement() {
  delete exp1;
  delete exp2;
}

QString EndStatement::toTree() { return "END"; }

QString PrintStatement::toTree() {
  QString tree("PRINT\n");
  tree.append(exp->toTree() + '\n');
  return tree;
}

PrintStatement::~PrintStatement() { delete exp; }
