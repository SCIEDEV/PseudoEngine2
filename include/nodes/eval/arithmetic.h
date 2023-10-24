#pragma once
#include "nodes/base.h"
#include "interpreter/types/types.h"

class IntegerNode : public Node {
private:
    const Interpreter::Integer valueInt;

public:
    IntegerNode(const Token &token);

    std::unique_ptr<NodeResult> evaluate(Interpreter::Context &ctx) override;
};

class RealNode : public Node {
private:
    const Interpreter::Real valueReal;

public:
    RealNode(const Token &token);

    std::unique_ptr<NodeResult> evaluate(Interpreter::Context &ctx) override;
};

class CharNode : public Node {
private:
    const Interpreter::Char valueChar;

public:
    CharNode(const Token &token);

    std::unique_ptr<NodeResult> evaluate(Interpreter::Context &ctx) override;
};

class StringNode : public Node {
private:
    const Interpreter::String valueStr;

public:
    StringNode(const Token &token);

    std::unique_ptr<NodeResult> evaluate(Interpreter::Context &ctx) override;
};

class DateNode : public Node {
private:
    const Interpreter::Date valueDate;

public:
    DateNode(const Token &token);

    std::unique_ptr<NodeResult> evaluate(Interpreter::Context &ctx) override;
};


class NegateNode : public UnaryNode {
public:
    using UnaryNode::UnaryNode;

    std::unique_ptr<NodeResult> evaluate(Interpreter::Context &ctx) override;
};

class ArithmeticOperationNode : public BinaryNode {
private:
    std::string op;

public:
    ArithmeticOperationNode(const Token &token, Node &left, Node &right);

    std::unique_ptr<NodeResult> evaluate(Interpreter::Context &ctx) override;
};
