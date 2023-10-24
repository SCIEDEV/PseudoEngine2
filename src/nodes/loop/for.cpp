#include "pch.h"

#include "interpreter/error.h"
#include "nodes/loop/control.h"
#include "nodes/loop/for.h"

ForLoopNode::ForLoopNode(const Token &token, const Token &identifier, Node &start, Node &stop, Node *step, Interpreter::Block *block)
    : Node(token),
    identifier(identifier),
    start(start),
    stop(stop),
    step(step),
    block(block)
{}

std::unique_ptr<NodeResult> ForLoopNode::evaluate(Interpreter::Context &ctx) {
    Interpreter::Variable *iterator = ctx.getVariable(identifier.value);
    if (iterator == nullptr) {
        iterator = new Interpreter::Variable(identifier.value, Interpreter::DataType::INTEGER, false, &ctx);
        ctx.addVariable(iterator);
    }

    if (iterator->type != Interpreter::DataType::INTEGER)
        throw Interpreter::RuntimeError(token, ctx, "Iterator variable must be of type INTEGER");


    auto startRes = start.evaluate(ctx);

    if (startRes->type != Interpreter::DataType::INTEGER)
        throw Interpreter::RuntimeError(token, ctx, "Start value of FOR loop iterator must be of type INTEGER");


    auto stopRes = stop.evaluate(ctx);

    if (stopRes->type != Interpreter::DataType::INTEGER)
        throw Interpreter::RuntimeError(token, ctx, "Stop value of FOR loop iterator must be of type INTEGER");


    Interpreter::int_t stepValue;
    if (step != nullptr) {
        auto stepRes = step->evaluate(ctx);

        if (stepRes->type != Interpreter::DataType::INTEGER)
            throw Interpreter::RuntimeError(token, ctx, "Step value of FOR loop iterator must be of type INTEGER");

        stepValue = stepRes->get<Interpreter::Integer>();
    } else {
        stepValue = 1;
    }

    Interpreter::Integer &iteratorValue = iterator->get<Interpreter::Integer>();

    Interpreter::int_t startValue = startRes->get<Interpreter::Integer>();
    Interpreter::int_t stopValue = stopRes->get<Interpreter::Integer>();

    bool stepNegative = stepValue < 0;

    for (iteratorValue = startValue;
        (stepNegative && iteratorValue.value >= stopValue) || (!stepNegative && iteratorValue.value <= stopValue);
        iteratorValue.value += stepValue
    ) {
        try {
            block->run(ctx);
        } catch (BreakErrSignal&) {
            break;
        } catch (ContinueErrSignal&) {
            continue;
        }
    }

    return std::make_unique<NodeResult>(nullptr, Interpreter::DataType::NONE);
}
