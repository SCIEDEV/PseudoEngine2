#include "pch.h"

#include "interpreter/error.h"
#include "nodes/selection/ifStatement.h"

IfConditionComponent::IfConditionComponent(Node *condition, Interpreter::Block &block)
    : condition(condition), block(block)
{}


IfStatementNode::IfStatementNode(const Token &token, std::vector<IfConditionComponent> &&components)
    : Node(token), components(std::move(components))
{}

std::unique_ptr<NodeResult> IfStatementNode::evaluate(Interpreter::Context &ctx) {
    for (IfConditionComponent &component : components) {
        if (component.condition == nullptr) {
            component.block.run(ctx);
            break;
        }

        std::unique_ptr<NodeResult> conditionRes = component.condition->evaluate(ctx);
        if (conditionRes->type != Interpreter::DataType::BOOLEAN)
            throw Interpreter::ConditionTypeError(token, ctx);

        if (conditionRes->get<Interpreter::Boolean>()) {
            component.block.run(ctx);
            break;
        }
    }

    return std::make_unique<NodeResult>(nullptr, Interpreter::DataType::NONE);
}
