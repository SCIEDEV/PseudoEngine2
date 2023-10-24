#include "pch.h"

#include "interpreter/error.h"
#include "nodes/variable/variable.h"
#include "nodes/variable/enum.h"

EnumDefineNode::EnumDefineNode(const Token &token, const Token &name, std::vector<std::string> &&values)
    : Node(token), name(name), values(std::move(values)) {}

std::unique_ptr<NodeResult> EnumDefineNode::evaluate(Interpreter::Context &ctx) {
    if (ctx.isIdentifierType(name, false))
        throw Interpreter::RedefinitionError(token, ctx, name.value);

    Interpreter::EnumTypeDefinition definition(name.value, std::move(values));
    ctx.createEnumDefinition(std::move(definition));
    return std::make_unique<NodeResult>(nullptr, Interpreter::DataType::NONE);
}
