#pragma once
#include "nodes/base.h"

class BooleanNode : public Node {
private:
    Interpreter::Boolean value;

public:
    BooleanNode(const Token &token);

    std::unique_ptr<NodeResult> evaluate(Interpreter::Context &ctx) override;
};

class ComparisonNode : public BinaryNode {
private:
    std::string op;

public:
    ComparisonNode(const Token &token, Node &left, Node &right);

    std::unique_ptr<NodeResult> evaluate(Interpreter::Context &ctx) override;
};
