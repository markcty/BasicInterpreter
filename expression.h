#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <QStack>
#include <QString>
#include <QtMath>

#include "basicinterpreter.h"
#include "environment.h"
#include "statement.h"

class Environment;

/*
 * Class: Expression
 * -----------------
 * This class is used to represent an expression Tree.
 * It contains three types of node: constant, identifier, compound
 */
class Expression {
 private:
  enum NodeType { CONSTANT, IDENTIFIER, COMPOUND, ERR };
  class Node {
   public:
    NodeType type = ERR;
    Node *left, *right;
    explicit Node(Node *left = nullptr, Node *right = nullptr);
    virtual int eval(const Environment &context) const = 0;
  };
  /*
   * Class: ConstantNode
   * ------------------
   * This subclass represents a constant integer expression node.
   */
  class ConstantNode : public Node {
   public:
    int value = 0;
    NodeType type = CONSTANT;
    int eval(const Environment &env) const override;
    explicit ConstantNode(const QString &token);
  };
  /*
   * Class: IdentifierNode
   * --------------------
   * This subclass represents a expression node corresponding to a variable.
   */
  class IdentifierNode : public Node {
   public:
    QString variable;
    NodeType type = IDENTIFIER;
    int eval(const Environment &env) const override;
    explicit IdentifierNode(const QString &token);
  };
  /*
   * Class: CompoundNode
   * ------------------
   * This subclass represents a compound expression node.
   */
  class CompoundNode : public Node {
   public:
    QString op;
    NodeType type = COMPOUND;
    int eval(const Environment &env) const override;
    explicit CompoundNode(const QString &c);
  };

  Node *root{};

 public:
  explicit Expression(QString exp);
  // evaluate the expression and return the result
  int eval(const Environment &context) const;
  QString toString() const;
};

#endif  // EXPRESSION_H
