#pragma once
#include "nodes/base.h"
#include "interpreter/scope/block.h"

class ForLoopNode : public Node {
private:
    const Token &identifier;
    Node &start, &stop, *step;
    Interpreter::Block *block;

public:
    ForLoopNode(const Token &token, const Token &identifier, Node &start, Node &stop, Node *step, Interpreter::Block *block);

    std::unique_ptr<NodeResult> evaluate(Interpreter::Context &ctx) override;
};
