#include "pch.h"

#include "interpreter/types/type_definitions.h"
#include "interpreter/types/types.h"
#include "interpreter/scope/block.h"

using namespace Interpreter;

Enum::Enum(const std::string &name) : definitionName(name) {}

void Enum::operator=(const Enum &other) {
    if (definitionName != other.definitionName) std::abort();
    idx = other.idx;
}

const EnumTypeDefinition &Enum::getDefinition(Context &ctx) const {
    return *ctx.getEnumDefinition(definitionName);
}

std::string Enum::getString(Context &ctx) const {
    const EnumTypeDefinition &definition = getDefinition(ctx);
    return definition.values[idx];
}

Pointer::Pointer(const std::string &name)
    : definitionName(name) {}

Pointer::Pointer(const Pointer &other)
    : ptr(other.ptr), varCtx(other.varCtx), definitionName(other.definitionName)
{}

void Pointer::setValue(Variable *value) {
    ptr = value;
    varCtx = value->parent;
    while (varCtx->isCompositeCtx) {
        varCtx = varCtx->getParent();
    }
}

void Pointer::operator=(const Pointer &other) {
    if (definitionName != other.definitionName) std::abort();
    ptr = other.ptr;
    varCtx = other.varCtx;
}

Variable *Pointer::getValue() const {
    return ptr;
}

const Context *Pointer::getCtx() const {
    return varCtx;
}

const PointerTypeDefinition &Pointer::getDefinition(Context &ctx) const {
    return *ctx.getPointerDefinition(definitionName);
}

Composite::Composite(const std::string &name, Context &parent)
    : definitionName(name),
    ctx(std::make_unique<Context>(&parent, definitionName, true))
{
    getDefinition(*ctx).initBlock.run(*ctx);
}

Composite::Composite(const Composite &other)
    : definitionName(other.definitionName),
    ctx(std::make_unique<Context>(*other.ctx))
{}

void Composite::operator=(const Composite &other) {
    if (definitionName != other.definitionName) std::abort();
    ctx->copyVariableData(*other.ctx);
}

DataHolder *Composite::getMember(const std::string &name) {
    Interpreter::Variable *var = ctx->getVariable(name);
    if (var != nullptr) return var;

    Interpreter::Array *arr = ctx->getArray(name);
    return arr;
}

const CompositeTypeDefinition &Composite::getDefinition(Interpreter::Context &ctx) const {
    return *ctx.getCompositeDefinition(definitionName);
}
