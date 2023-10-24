#include "pch.h"

#include "interpreter/error.h"
#include "nodes/loop/control.h"
#include "nodes/loop/repeatUntil.h"

RepeatUntilNode::RepeatUntilNode(const Token &token, Node &condition, Interpreter::Block &block)
    : UnaryNode(token, condition), block(block)
{}

std::unique_ptr<NodeResult> RepeatUntilNode::evaluate(Interpreter::Context &ctx) {
    while (true) {
        try {
            block.run(ctx);
        } catch (BreakErrSignal&) {
            break;
        } catch (ContinueErrSignal&) {
            continue;
        }

        auto conditionRes = node.evaluate(ctx);
        
        if (conditionRes->type != Interpreter::DataType::BOOLEAN)
            throw Interpreter::ConditionTypeError(token, ctx);

        if (conditionRes->get<Interpreter::Boolean>()) break;
    }

    return std::make_unique<NodeResult>(nullptr, Interpreter::DataType::NONE);
}
