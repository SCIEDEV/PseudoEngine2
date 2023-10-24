#pragma once
#include "nodes/base.h"
#include "interpreter/scope/block.h"

class CompositeDefineNode : public Node {
private:
    const Token &name;
    Interpreter::Block &initBlock;

public:
    CompositeDefineNode(const Token &token, const Token &name, Interpreter::Block &initBlock);

    std::unique_ptr<NodeResult> evaluate(Interpreter::Context &ctx) override;
};
