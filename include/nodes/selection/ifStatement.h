#pragma once
#include <vector>
#include "nodes/base.h"
#include "interpreter/scope/block.h"

struct IfConditionComponent {
    // nullptr represents ELSE component
    Node *condition;
    Interpreter::Block &block;

    IfConditionComponent(Node *condition, Interpreter::Block &block);
};

class IfStatementNode : public Node {
private:
    std::vector<IfConditionComponent> components;

public:
    IfStatementNode(const Token &token, std::vector<IfConditionComponent> &&components);

    std::unique_ptr<NodeResult> evaluate(Interpreter::Context &ctx) override;
};
