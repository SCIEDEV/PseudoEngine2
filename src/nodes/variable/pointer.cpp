#include "pch.h"

#include "interpreter/error.h"
#include "nodes/variable/variable.h"
#include "nodes/variable/pointer.h"

PointerDefineNode::PointerDefineNode(const Token &token, const Token &name, const Token &type)
    : Node(token), name(name), type(type) {}

std::unique_ptr<NodeResult> PointerDefineNode::evaluate(Interpreter::Context &ctx) {
    Interpreter::DataType pointerType = ctx.getType(type);
    if (pointerType == Interpreter::DataType::NONE)
        throw Interpreter::NotDefinedError(token, ctx, "Type '" + type.value + "'");

    if (ctx.isIdentifierType(name, false))
        throw Interpreter::RedefinitionError(token, ctx, name.value);

    Interpreter::PointerTypeDefinition definition(name.value, pointerType);
    ctx.createPointerDefinition(std::move(definition));
    return std::make_unique<NodeResult>(nullptr, Interpreter::DataType::NONE);
}

PointerAssignNode::PointerAssignNode(
    const Token &token,
    std::unique_ptr<AbstractVariableResolver> &&pointerResolver,
    std::unique_ptr<AbstractVariableResolver> &&valueResolver
) : Node(token),
    pointerResolver(std::move(pointerResolver)),
    valueResolver(std::move(valueResolver))
{}

std::unique_ptr<NodeResult> PointerAssignNode::evaluate(Interpreter::Context &ctx) {
    auto &pointerHolder = pointerResolver->resolve(ctx);
    if (pointerHolder.isArray())
        throw Interpreter::ArrayDirectAccessError(token, ctx);

    auto &valueHolder = valueResolver->resolve(ctx);
    if (valueHolder.isArray())
        throw Interpreter::RuntimeError(token, ctx, "Cannot store pointer to array");
    
    auto &pointerVar = *static_cast<Interpreter::Variable*>(&pointerHolder);
    auto &value = *static_cast<Interpreter::Variable*>(&valueHolder);

    if (pointerVar.type.type != Interpreter::DataType::POINTER)
        throw Interpreter::RuntimeError(token, ctx, "Cannot assign pointer to variable not of type pointer");

    auto &pointer = pointerVar.get<Interpreter::Pointer>();
    auto &pointerType = pointer.getDefinition(ctx).type;
    if (pointerType != value.type)
        throw Interpreter::RuntimeError(token, ctx, "Assignment Error: Incompatible data types");
    
    Interpreter::Variable *v = value.getReference();
    if (v == nullptr) v = &value;
    pointer.setValue(v);

    return std::make_unique<NodeResult>(nullptr, Interpreter::DataType::NONE);
}
