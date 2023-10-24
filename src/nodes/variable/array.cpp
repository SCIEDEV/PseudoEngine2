#include "pch.h"

#include "interpreter/error.h"
#include "nodes/variable/variable.h"
#include "nodes/variable/array.h"

ArrayDeclareNode::ArrayDeclareNode(const Token &token, std::vector<const Token*> &&identifiers, const Token &type, std::vector<Node*> &&bounds)
    : Node(token),
    identifiers(identifiers),
    type(type),
    bounds(std::move(bounds))
{}

std::unique_ptr<NodeResult> ArrayDeclareNode::evaluate(Interpreter::Context &ctx) {
    if (bounds.size() % 2 != 0 || bounds.size() == 0) std::abort();

    for (auto identifier : identifiers) {
        if (ctx.getArray(identifier->value, false) != nullptr)
            throw Interpreter::RedeclarationError(token, ctx, identifier->value);
    }

    std::vector<Interpreter::ArrayDimension> dimensions;

    for (size_t i = 0; i < bounds.size(); i += 2) {
        auto lowerRes = bounds[i]->evaluate(ctx);
        if (lowerRes->type != Interpreter::DataType::INTEGER)
            throw Interpreter::RuntimeError(bounds[i]->getToken(), ctx, "Array indices must be of type INTEGER");

        auto upperRes = bounds[i + 1]->evaluate(ctx);
        if (upperRes->type != Interpreter::DataType::INTEGER)
            throw Interpreter::RuntimeError(bounds[i + 1]->getToken(), ctx, "Array indices must be of type INTEGER");

        const Interpreter::int_t &lower = lowerRes->get<Interpreter::Integer>().value;
        const Interpreter::int_t &upper = upperRes->get<Interpreter::Integer>().value;

        if (upper < lower)
            throw Interpreter::RuntimeError(bounds[i + 1]->getToken(), ctx, "Array upper bound must be greater than lower bound");

        dimensions.emplace_back((Interpreter::int_t) (i / 2), lower, upper);
    }

    for (auto identifier : identifiers) {
        Interpreter::DataType dataType = ctx.getType(type);
        if (dataType.type == Interpreter::DataType::NONE)
            throw Interpreter::NotDefinedError(token, ctx, "Type '" + type.value + "'");

        auto array = std::make_unique<Interpreter::Array>(identifier->value, dataType, dimensions);
        array->init(ctx);
        ctx.addArray(std::move(array));
    }

    return std::make_unique<NodeResult>(nullptr, Interpreter::DataType::NONE);
}
