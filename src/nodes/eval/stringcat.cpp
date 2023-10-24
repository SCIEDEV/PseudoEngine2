#include "pch.h"

#include "interpreter/error.h"
#include "nodes/eval/stringcat.h"

std::unique_ptr<NodeResult> StringConcatenationNode::evaluate(Interpreter::Context &ctx) {
    auto leftRes = left.evaluate(ctx);
    auto rightRes = right.evaluate(ctx);

    if (!leftRes->data->isPrimitive() || !rightRes->data->isPrimitive())
        throw Interpreter::TypeOperationError(token, ctx, "'&'");

    auto leftStr = static_cast<const Interpreter::Primitive*>(leftRes->data.get())->toString();
    auto rightStr = static_cast<const Interpreter::Primitive*>(rightRes->data.get())->toString();

    auto res = (*leftStr) & (*rightStr);

    return std::make_unique<NodeResult>(std::move(res), Interpreter::DataType::STRING);
}
