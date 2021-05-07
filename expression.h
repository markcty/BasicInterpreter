#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <QDebug>
#include <QList>
#include <QStack>
#include <QString>
#include <QtMath>
#include <exception>

#include "basicinterpreter.h"
#include "environment.h"
#include "qstringexception.h"
#include "statement.h"
#include "declarations.h"

/*
 * Class: Expression
 * -----------------
 * This class is used to represent an expression Tree.
 * It contains three types of node: constant, identifier, compound
 */
class Expression {
 private:
  enum NodeType { CONSTANT, IDENTIFIER, COMPOUND };

  /*
   * Class: Node
   * -----------------
   * A virtual class which all types of node inherit from
   */
  class Node {
   public:
    NodeType type;
    Node *left, *right;
    explicit Node(Node *left = nullptr, Node *right = nullptr);
    virtual int eval(const Environment &context) const = 0;
    virtual int getConstant();
    virtual QString getVariable();
    virtual QString getOperator();
    virtual ~Node() = default;
  };

  /*
   * Class: ConstantNode
   * ------------------
   * This subclass represents a constant integer expression node.
   */
  class ConstantNode : public Node {
   public:
    int value = 0;
    int eval(const Environment &env) const override;
    explicit ConstantNode(const QString &token);
    int getConstant() override;
    ~ConstantNode() = default;
  };

  /*
   * Class: IdentifierNode
   * --------------------
   * This subclass represents a expression node corresponding to a variable.
   */
  class IdentifierNode : public Node {
   public:
    QString variable;
    int eval(const Environment &env) const override;
    explicit IdentifierNode(const QString &token);
    QString getVariable() override;
    ~IdentifierNode() = default;
  };

  /*
   * Class: CompoundNode
   * -------------------
   * This subclass represents a compound expression node.
   */
  class CompoundNode : public Node {
   public:
    QString op;
    int eval(const Environment &env) const override;
    explicit CompoundNode(const QString &c);
    QString getOperator() override;
    ~CompoundNode();
  };

  Node *root = nullptr;

 public:
  explicit Expression(QString exp);

  // evaluate the expression and return the result
  int eval(const Environment &context) const;

  // return the original statement
  QString toString() const;

  // return the expression tree of the statement
  QString toTree() const;
  ~Expression();
};

#endif  // EXPRESSION_H
