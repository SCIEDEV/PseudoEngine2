#pragma once
#include "nodes/base.h"

class CastNode : public UnaryNode {
private:
    Interpreter::DataType target;

public:
    CastNode(const Token &token, Node &node, Interpreter::DataType target);

    std::unique_ptr<NodeResult> evaluate(Interpreter::Context &ctx) override;
};
