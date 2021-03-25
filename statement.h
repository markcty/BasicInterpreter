#ifndef STATEMENT_H
#define STATEMENT_H

#include <QString>
#include <QStringList>

#include "basicinterpreter.h"
#include "environment.h"
#include "expression.h"

class Expression;

enum StatementType { REM, LET, PRINT, INPUT, GOTO, IF, END, NONE };

class Statement {
 public:
  StatementType type = NONE;
  virtual QString toString() const;
  virtual QString getVariable();
  virtual int getConstant();
  virtual int getLineNumber();
  virtual QString getOperator();
  virtual Expression *getFirstExp();
  virtual Expression *getSecondExp();
  virtual QString toTree() = 0;
  virtual ~Statement() = default;

 protected:
  QString line;
};

class RemStatement : public Statement {
 public:
  explicit RemStatement(const QString &l);
  QString toTree() override;
  ~RemStatement() = default;
};

class LetStatement : public Statement {
 private:
  QString variable;
  Expression *exp;

 public:
  explicit LetStatement(QString l);
  QString getVariable() override;
  Expression *getFirstExp() override;
  QString toTree() override;
  ~LetStatement();
};

class PrintStatement : public Statement {
 private:
  Expression *exp;

 public:
  explicit PrintStatement(QString l);
  Expression *getFirstExp() override;
  QString toTree() override;
  ~PrintStatement();
};

class InputStatement : public Statement {
 private:
  QString variable;
  int value = 0;

 public:
  explicit InputStatement(QString l);
  QString getVariable() override;
  QString toTree() override;
  ~InputStatement() = default;
};

class GotoStatement : public Statement {
 private:
  int lineNumber;

 public:
  explicit GotoStatement(QString l);
  int getLineNumber() override;
  QString toTree() override;
  ~GotoStatement() = default;
};

class IfStatement : public Statement {
 private:
  QString op;
  Expression *exp1, *exp2;
  int lineNumber;

 public:
  explicit IfStatement(QString l);
  Expression *getFirstExp() override;
  Expression *getSecondExp() override;
  QString getOperator() override;
  int getLineNumber() override;
  QString toTree() override;
  ~IfStatement();
};

class EndStatement : public Statement {
 public:
  explicit EndStatement(const QString &l);
  QString toTree() override;
  ~EndStatement() = default;
};

#endif  // STATEMENT_H
