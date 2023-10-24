#include "pch.h"

#include "interpreter/error.h"
#include "nodes/loop/control.h"
#include "nodes/loop/while.h"

WhileLoopNode::WhileLoopNode(const Token &token, Node &condition, Interpreter::Block &block)
    : UnaryNode(token, condition), block(block)
{}

std::unique_ptr<NodeResult> WhileLoopNode::evaluate(Interpreter::Context &ctx) {
    while (true) {
        auto conditionRes = node.evaluate(ctx);

        if (conditionRes->type != Interpreter::DataType::BOOLEAN)
            throw Interpreter::ConditionTypeError(token, ctx);

        if (!conditionRes->get<Interpreter::Boolean>()) break;

        try {
            block.run(ctx);
        } catch (BreakErrSignal&) {
            break;
        } catch (ContinueErrSignal&) {
            continue;
        }
    }

    return std::make_unique<NodeResult>(nullptr, Interpreter::DataType::NONE);
}
