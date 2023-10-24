#include "pch.h"

#include "interpreter/error.h"
#include "nodes/cast.h"

CastNode::CastNode(const Token &token, Node &node, Interpreter::DataType target)
    : UnaryNode(token, node), target(target)
{}

std::unique_ptr<NodeResult> CastNode::evaluate(Interpreter::Context &ctx) {
    auto value = node.evaluate(ctx);

    if (!value->data->isPrimitive())
        throw Interpreter::TypeOperationError(token, ctx, "Cast");
    if (value->type == target) return value;

    auto pvalue = static_cast<const Interpreter::Primitive*>(value->data.get());

    std::unique_ptr<Interpreter::Value> result;
    switch(target.type) {
        case Interpreter::DataType::INTEGER:
            result = pvalue->toInteger();
            break;
        case Interpreter::DataType::REAL:
            result = pvalue->toReal();
            break;
        case Interpreter::DataType::BOOLEAN:
            result = pvalue->toBoolean();
            break;
        case Interpreter::DataType::CHAR:
            result = pvalue->toChar();
            break;
        case Interpreter::DataType::STRING:
            result = pvalue->toString();
            break;
        case Interpreter::DataType::DATE:
        case Interpreter::DataType::ENUM:
        case Interpreter::DataType::POINTER:
        case Interpreter::DataType::COMPOSITE:
            throw Interpreter::TypeOperationError(token, ctx, "Cast");
        case Interpreter::DataType::NONE:
            std::abort();
    }

    return std::make_unique<NodeResult>(std::move(result), target);
}
