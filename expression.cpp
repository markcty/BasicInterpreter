#include "expression.h"
Expression::Expression(QString exp) {
  exp.remove(" ");
  // return the precedence of an operator, -1 if not an operator
  auto getPre = [](QChar c) {
    if (c == '(' || c == ')') return 2;
    if (c == '*' || c == '/') return 1;
    if (c == '+' || c == '-') return 0;
    return -1;
  };

  // tokenize
  // wrap the expression in a parenthesis can simplify parsing
  exp.push_front("(");
  exp.push_back(")");
  // add a 0 before - to dealth with negative number
  int j = 0;
  while ((j = exp.indexOf('-', j)) != -1) {
    if (exp[j - 1] == '(') exp.insert(j - 1, QString(" "));
    ++j;
  }
  int i = 0, nexti = 0, length = exp.length();
  QStringList tokens;
  while (i < length) {
    if (getPre(exp[i]) >= 0)
      tokens.push_back(QString(exp[i++]));
    else {
      nexti = i;
      if (exp[i].isDigit()) {
        // is constantf
        while (exp[nexti].isDigit()) nexti++;
        tokens.push_back(exp.mid(i, nexti - i));
      } else {
        // is variable
        while (exp[nexti].isLetterOrNumber()) nexti++;
        tokens.push_back(exp.mid(i, nexti - i));
      }
      i = nexti;
    }
  }

  // parse
  QStack<QChar> operators;
  QStack<Node *> operands;

  for (const QString &token : tokens) {
    if (getPre(token[0]) >= 0) {
      // current token is an operator or a parenthesis
      QChar op = token[0];
      QChar top;
      Node *node;
      if (op == ')')
        while ((top = operators.pop()) != '(') {
          node = new CompoundNode(top);
          node->right = operands.pop();
          node->left = operands.pop();
          operands.push(node);
        }
      else if (op == '(')
        operators.push('(');
      else {
        while (!operators.empty() && operators.top() != '(' &&
               getPre(operators.top()) >= getPre(op)) {
          top = operators.pop();
          node = new CompoundNode(top);
          node->right = operands.pop();
          node->left = operands.pop();
          operands.push(node);
        }
        operators.push(op);
      }
    } else {
      // current token is a constant or an identifier
      Node *node;
      if (token[0].isDigit())
        node = new ConstantNode(token);
      else if (token[0].isLetter())
        node = new IdentifierNode(token);
      operands.push(node);
    }
  }
  root = operands.pop();
}
QString Expression::toString() const { return QString(); }

int Expression::eval(const Environment &context) const {
  return root->eval(context);
}

int Expression::IdentifierNode::eval(const Environment &env) const {
  return env.getValue(variable);
}

Expression::IdentifierNode::IdentifierNode(const QString &token)
    : variable(QString(token)) {}

int Expression::ConstantNode::eval(const Environment &env) const {
  return value;
}

Expression::ConstantNode::ConstantNode(const QString &token)
    : value(token.toInt()) {}
int Expression::CompoundNode::eval(const Environment &env) const {
  int lv = left->eval(env), rv = right->eval(env);
  if (op == '+') return lv + rv;
  if (op == '-') return lv - rv;
  if (op == '*') return lv * rv;
  if (op == '/') return lv / rv;
  return 0;
}

Expression::CompoundNode::CompoundNode(const QChar &c) : op(c) {}

Expression::Node::Node(Expression::Node *left, Expression::Node *right)
    : left(left), right(right) {}
