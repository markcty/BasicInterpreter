#include "statement.h"
RemStatement::RemStatement(const QString &l) {
  statementType = REM;
  line = l;
}

LetStatement::LetStatement(QString l) {
  line = l;
  statementType = LET;
}

QString LetStatement::getVariable() { return variable; }

Expression *LetStatement::getFirstExp() { return exp; }

PrintStatement::PrintStatement(QString l) {
  line = l;
  statementType = PRINT;
}

Expression *PrintStatement::getFirstExp() { return exp; }

InputStatement::InputStatement(QString l) {
  line = l;
  statementType = INPUT;
}

QString InputStatement::getVariable() { return variable; }

GotoStatement::GotoStatement(QString l) {
  line = l;
  statementType = GOTO;
}

int GotoStatement::getLineNumber() { return lineNumber; }

IfStatement::IfStatement(QString l) {
  line = l;
  statementType = IF;
}

Expression *IfStatement::getFirstExp() { return exp1; }

Expression *IfStatement::getSecondExp() { return exp2; }

QString IfStatement::getOperator() { return op; }

int IfStatement::getLineNumber() { return lineNumber; }

void IfStatement::parse() {
  QString l = line;
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
}

EndStatement::EndStatement(const QString &l) {
  line = l;
  statementType = END;
}

QString Statement::toString() const { return line; }

QString Statement::getVariable() { return ""; }

int Statement::getConstant() { return 0; }

int Statement::getLineNumber() { return 0; }

QString Statement::getOperator() { return " "; }

Expression *Statement::getFirstExp() { return nullptr; }

Expression *Statement::getSecondExp() { return nullptr; }

QString RemStatement::toTree() {
  if (statementType == ERR) return "ERR";
  QString tree("REM\n");
  tree.append("    " + line.mid(line.indexOf(" ") + 1) + "\n");
  return tree;
}

void RemStatement::parse() {}

QString LetStatement::toTree() {
  if (statementType == ERR) return "ERR";
  QString tree("LET =\n");
  tree.append("    " + variable + '\n');
  tree.append(exp->toTree() + '\n');
  return tree;
}

VariableType LetStatement::getType() const { return variableType; }

QString LetStatement::getVal() const { return val; }

void LetStatement::parse() {
  QString l = line;
  l = l.remove("LET").simplified();

  if (l.indexOf("=") == -1) throw QStringException("Invalid Statement");
  auto part = l.split("=");
  if (part.size() != 2) throw QStringException("Invalid Statement");
  variable = part[0].simplified();

  // string
  int i = l.indexOf("\"");
  if (i != -1) {
    variableType = STR;
    val = l.mid(i + 1, l.length() - i - 2);
    if (val.indexOf("\"") != -1) throw QStringException("Invalid Statement");
  }
  // int
  else {
    variableType = INT;
    exp = new Expression(part[1].simplified());
  }
}

LetStatement::~LetStatement() {
  if (variableType == INT) delete exp;
}

QString InputStatement::toTree() {
  if (statementType == ERR) return "ERR";
  QString tree("INPUT =\n");
  tree.append("    " + variable + '\n');
  return tree;
}

void InputStatement::parse() {
  QString l = line;
  l = l.remove("INPUT").simplified();
  variable = l;
}

QString GotoStatement::toTree() {
  if (statementType == ERR) return "ERR";
  QString tree("GOTO\n");
  tree.append("    " + QString::number(lineNumber) + '\n');
  return tree;
}

void GotoStatement::parse() {
  QString l = line;
  l = l.remove("GOTO").simplified();
  bool ok = false;
  lineNumber = l.toInt(&ok);
  if (!ok) throw QStringException("Invalid Statement");
}

QString IfStatement::toTree() {
  if (statementType == ERR) return "ERR";
  QString tree("IF THEN\n");
  tree.append(exp1->toTree() + '\n');
  tree.append("    " + op + '\n');
  tree.append(exp2->toTree() + '\n');
  tree.append("    " + QString::number(lineNumber) + '\n');
  return tree;
}

IfStatement::~IfStatement() {
  delete exp1;
  delete exp2;
}

QString EndStatement::toTree() {
  if (statementType == ERR) return "ERR";
  return "END";
}

void EndStatement::parse() {}

QString PrintStatement::toTree() {
  if (statementType == ERR) return "ERR";
  QString tree("PRINT\n");
  tree.append(exp->toTree() + '\n');
  return tree;
}

void PrintStatement::parse() {
  QString l = line;
  l = l.remove("PRINT").simplified();
  exp = new Expression(l);
}

PrintStatement::~PrintStatement() { delete exp; }

InvalidStatement::InvalidStatement(const QString &l) {
  line = l;
  statementType = ERR;
}

QString InvalidStatement::toTree() {
  if (statementType == ERR) return "ERR";
  throw QStringException("Can't call toTree of an invalid statement");
}

void InvalidStatement::parse() { throw QStringException("Invalid Statement"); }
