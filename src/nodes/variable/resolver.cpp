#include "pch.h"

#include "interpreter/error.h"
#include "nodes/variable/resolver.h"


AbstractVariableResolver::AbstractVariableResolver(const Token &token)
    : token(token) {}

PointerDereferencer::PointerDereferencer(const Token &token, std::unique_ptr<AbstractVariableResolver> &&resolver)
    : AbstractVariableResolver(token), resolver(std::move(resolver)) {}

Interpreter::DataHolder &PointerDereferencer::resolve(Interpreter::Context &ctx) const {
    auto &dh = resolver->resolve(ctx);
    if (dh.isArray())
        throw Interpreter::InvalidUsageError(token, ctx, "'^' operator: Attempting to dereference non-pointer");

    auto &var = *static_cast<Interpreter::Variable*>(&dh);
    if (var.type != Interpreter::DataType::POINTER)
        throw Interpreter::InvalidUsageError(token, ctx, "'^' operator: Attempting to dereference non-pointer");

    Interpreter::Pointer &ptr = var.get<Interpreter::Pointer>();
    const Interpreter::Context *ptrCtx = ptr.getCtx();
    Interpreter::Context *tempCtx = &ctx;
    while (tempCtx != ptrCtx) {
        tempCtx = tempCtx->getParent();
        if (tempCtx == nullptr)
            throw Interpreter::RuntimeError(token, ctx, "Attempting to access deleted object from pointer '" + var.name + "'");
    }

    Interpreter::Variable *ptrVar = ptr.getValue();
    if (ptrVar == nullptr)
        throw Interpreter::RuntimeError(token, ctx, "Attempt to access uninitalized pointer");

    return *ptrVar;
}

CompositeResolver::CompositeResolver(const Token &token, std::unique_ptr<AbstractVariableResolver> &&resolver, const Token &member)
    : AbstractVariableResolver(token),
    resolver(std::move(resolver)),
    member(member)
{}

Interpreter::DataHolder &CompositeResolver::resolve(Interpreter::Context &ctx) const {
    auto &dh = resolver->resolve(ctx);
    if (dh.isArray())
        throw Interpreter::InvalidUsageError(token, ctx, "'.' operator: Variable is not a composite type");

    auto &var = *static_cast<Interpreter::Variable*>(&dh);
    if (var.type != Interpreter::DataType::COMPOSITE)
        throw Interpreter::InvalidUsageError(token, ctx, "'.' operator: Variable is not a composite type");
    
    auto &composite = var.get<Interpreter::Composite>();
    auto *memberPtr = composite.getMember(member.value);
    if (memberPtr == nullptr)
        throw Interpreter::RuntimeError(token, ctx, "Type '" + composite.definitionName + "' has no member '" + member.value + "'");
    
    return *memberPtr;
}

ArrayElementResolver::ArrayElementResolver(const Token &token, std::unique_ptr<AbstractVariableResolver> &&resolver, std::vector<Node*> &&indices)
    : AbstractVariableResolver(token),
    indices(std::move(indices)),
    resolver(std::move(resolver))
{}

Interpreter::DataHolder &ArrayElementResolver::resolve(Interpreter::Context &ctx) const {
    auto &dh = resolver->resolve(ctx);
    if (!dh.isArray())
        throw Interpreter::RuntimeError(token, ctx, "Attempting to index non-array variable '" + dh.name + "'");

    Interpreter::Array &arr = *static_cast<Interpreter::Array*>(&dh);

    if (indices.size() != arr.dimensions.size())
        throw Interpreter::RuntimeError(token, ctx, "Invalid number of indices");

    std::vector<Interpreter::int_t> evaluatedIndices;
    evaluatedIndices.reserve(indices.size());

    for (size_t i = 0; i < indices.size(); i++) {
        Node *index = indices[i];
        auto result = index->evaluate(ctx);

        if (result->type != Interpreter::DataType::INTEGER)
            throw Interpreter::RuntimeError(index->getToken(), ctx, "Array indices must be of type INTEGER");

        Interpreter::int_t x = result->get<Interpreter::Integer>().value;
        if (!arr.dimensions[i].isValidIndex(x))
            throw Interpreter::RuntimeError(index->getToken(), ctx, "Index out of bounds");

        evaluatedIndices.push_back(x);
    }

    Interpreter::Variable &var = arr.getElement(evaluatedIndices);
    return var;
}

Interpreter::DataHolder &SimpleVariableSource::resolve(Interpreter::Context &ctx) const {
    Interpreter::Variable *var = ctx.getVariable(token.value);
    if (var != nullptr) return *var;

    Interpreter::Array *arr = ctx.getArray(token.value);
    if (arr != nullptr)
        return *arr;

    throw Interpreter::NotDefinedError(token, ctx, "Identifier '" + token.value + "'");
}

const std::string &SimpleVariableSource::getName() const {
    return token.value;
}

const Token &SimpleVariableSource::getToken() const {
    return token;
}
