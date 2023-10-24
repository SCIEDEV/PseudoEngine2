#pragma once
#include "nodes/base.h"
#include "interpreter/scope/block.h"

class WhileLoopNode : public UnaryNode {
private:
    Interpreter::Block &block;

public:
    WhileLoopNode(const Token &token, Node &condition, Interpreter::Block &block);

    std::unique_ptr<NodeResult> evaluate(Interpreter::Context &ctx) override;
};
