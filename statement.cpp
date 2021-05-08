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

QString Statement::getVariable() {
  throw QStringException("unimplemented function");
}

int Statement::getConstant() {
  throw QStringException("unimplemented function");
}

int Statement::getLineNumber() {
  throw QStringException("unimplemented function");
}

QString Statement::getOperator() {
  throw QStringException("unimplemented function");
}

Expression *Statement::getFirstExp() {
  throw QStringException("unimplemented function");
}

Expression *Statement::getSecondExp() {
  throw QStringException("unimplemented function");
}

bool Statement::validifyVar(QString var) {
  if (!var[0].isLetter()) return false;
  for (auto c : var)
    if (!c.isLetterOrNumber()) return false;
  return true;
}

QString RemStatement::toTree() {
  if (statementType == ERR) return "ERR\n";
  QString tree("REM\n");
  tree.append("    " + line.mid(line.indexOf(" ") + 1) + "\n");
  return tree;
}

void RemStatement::parse() {}

QString LetStatement::toTree() {
  if (statementType == ERR) return "ERR\n";
  if (variableType == STR) return "LET = \"" + val + "\"\n";
  QString tree("LET =\n");
  tree.append("    " + variable + '\n');
  tree.append(exp->toTree() + '\n');
  return tree;
}

VariableType LetStatement::getType() const { return variableType; }

QString LetStatement::getVal() const { return val; }

void LetStatement::parse() {
  auto extractStr = [](QString s) {
    if (!((s.front() == '"' && s.back() == '"') ||
          (s.front() == '\'' && s.back() == '\'')))
      throw QStringException("Invalid Statement");
    s = s.mid(1, s.length() - 2);
    if (s.indexOf('"') != -1 || s.indexOf('\'') != -1)
      throw QStringException("Invalid Statement");
    return s;
  };

  QString l = line;
  l = l.remove("LET").simplified();

  if (l.indexOf("=") == -1) throw QStringException("Invalid Statement");
  auto part = l.split("=");
  if (part.size() != 2) throw QStringException("Invalid Statement");
  variable = part[0].simplified();

  if (!validifyVar(variable)) throw QStringException("Invalid Variable name");

  // string
  if (l.indexOf("\"") != -1 || l.indexOf("'") != -1) {
    variableType = STR;
    val = extractStr(part[1].simplified());
  } else if (l.indexOf("'") != -1) {
    int i = l.indexOf("'");
    if (l.back() != '\'') throw QStringException("Invalid Statement");
    variableType = STR;
    val = l.mid(i + 1, l.length() - i - 2);
    if (val.indexOf("\"") != -1 || val.indexOf("'") != -1)
      throw QStringException("Invalid Statement");
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
  if (statementType == ERR) return "ERR\n";
  QString tree("INPUT =\n");
  tree.append("    " + variable + '\n');
  return tree;
}

void InputStatement::parse() {
  QString l = line;
  l = l.remove("INPUT").simplified();
  variable = l;

  if (!validifyVar(variable)) throw QStringException("Invalid Variable name");
}

QString GotoStatement::toTree() {
  if (statementType == ERR) return "ERR\n";
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
  if (statementType == ERR) return "ERR\n";
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
  if (statementType == ERR) return "ERR\n";
  return "END";
}

void EndStatement::parse() {}

QString PrintStatement::toTree() {
  if (statementType == ERR) return "ERR\n";
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
  if (statementType == ERR) return "ERR\n";
  throw QStringException("Can't call toTree of an invalid statement");
}

void InvalidStatement::parse() { throw QStringException("Invalid Statement"); }

PrintfStatement::PrintfStatement(const QString &l) {
  line = l;
  statementType = PRINTF;
}

QString PrintfStatement::toTree() {
  if (statementType == ERR) return "ERR\n";
  QString tree("PRINTF\n");
  tree.append("    " + line.mid(line.indexOf(" ") + 1) + "\n");
  return tree;
}

void PrintfStatement::parse() {
  auto l = line;
  l.remove("PRINTF ");
  auto part = l.split(",");
  format = part[0].simplified();
  if (!((format.front() == '"' && format.back() == '"') ||
        (format.front() == '\'' && format.back() == '\'')))
    throw QStringException("Invalid Statement");
  format = format.mid(1, format.length() - 2);
  if (format.indexOf('"') != -1 || format.indexOf('\'') != -1)
    throw QStringException("Invalid Statement");

  int count = 0, j = 0;
  while ((j = format.indexOf("{}", j)) != -1) {
    count++;
    j++;
  }
  if (part.size() - 1 != count) throw QStringException("Invalid Statement");
  for (int i = 1; i < part.size(); i++) args.push_back(part[i].simplified());
}

QString PrintfStatement::compose(const Environment &env) {
  auto extractStr = [](QString s) {
    if (!((s.front() == '"' && s.back() == '"') ||
          (s.front() == '\'' && s.back() == '\'')))
      throw QStringException("Invalid Statement");
    s = s.mid(1, s.length() - 2);
    if (s.indexOf('"') != -1 || s.indexOf('\'') != -1)
      throw QStringException("Invalid Statement");
    return s;
  };

  QString s = format;
  for (auto &arg : args) {
    QString args;
    if (arg.front() == '\'' || arg.front() == '"') {
      args = extractStr(arg);
    } else if (arg.front().isDigit()) {
      bool ok = true;
      arg.toInt(&ok);
      if (!ok) throw QStringException("compose error");
      args = arg;
    } else {
      switch (env.getType(arg)) {
        case STR:
          args = env.getStrValue(arg);
          break;
        case INT:
          args = QString::number(env.getIntValue(arg));
          break;
      }
    }
    int i = s.indexOf("{}");
    s.remove(i, 2);
    s.insert(i, args);
  }

  return s;
}

InputsStatement::InputsStatement(QString l) {
  line = l;
  statementType = INPUTS;
}

QString InputsStatement::getVariable() { return variable; }

QString InputsStatement::toTree() {
  if (statementType == ERR) return "ERR\n";
  QString tree("INPUTS =\n");
  tree.append("    " + variable + '\n');
  return tree;
}

void InputsStatement::parse() {
  QString l = line;
  l = l.remove("INPUTS").simplified();
  variable = l;

  if (!validifyVar(variable)) throw QStringException("Invalid Variable name");
}
