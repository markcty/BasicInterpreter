#include "statement.h"
RemStatement::RemStatement(const QString &l) {
  type = REM;
  line = l;
}

LetStatement::LetStatement(QString l) {
  line = l;
  l = l.remove("LET").simplified();
  auto part = l.split("=");
  variable = part[0].simplified();
  exp = new Expression(part[1].simplified());
  type = LET;
}

QString LetStatement::getVariable() { return variable; }

Expression *LetStatement::getFirstExp() { return exp; }

// PrintStatement::PrintStatement(const QString &l) {
//  line = l;
//  exp = new Expression(l);
//  type = PRINT;
//}
//
// InputStatement::InputStatement(const QString &l) {
//  line = l;
//  variable = l;
//  type = INPUT;
//}

GotoStatement::GotoStatement(QString l) {
  line = l;
  l = l.remove("GOTO").simplified();
  lineNumber = l.toInt();
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
  auto split_op = l.split(op);
  exp1 = new Expression(split_op[0].simplified());
  auto split_then = split_op[1].split("THEN");
  exp2 = new Expression(split_then[0].simplified());
  lineNumber = split_then[1].toInt();
  type = IF;
}

Expression *IfStatement::getFirstExp() { return exp1; }

Expression *IfStatement::getSecondExp() { return exp2; }

int IfStatement::getLineNumber() { return lineNumber; }
EndStatement::EndStatement(const QString &l) {
  line = l;
  type = END;
}

QString Statement::toString() const { return line; }

QString Statement::getVariable() { return ""; }

int Statement::getConstant() { return 0; }

int Statement::getLineNumber() { return 0; }

QChar Statement::getOperator() { return ' '; }

Expression *Statement::getFirstExp() { return nullptr; }

Expression *Statement::getSecondExp() { return nullptr; }
