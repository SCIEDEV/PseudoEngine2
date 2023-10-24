#include "pch.h"

#include "interpreter/error.h"
#include "nodes/variable/variable.h"

DeclareNode::DeclareNode(const Token &token, std::vector<const Token*> &&identifiers, const Token &type)
    : Node(token), identifiers(std::move(identifiers)), type(type)
{}

std::unique_ptr<NodeResult> DeclareNode::evaluate(Interpreter::Context &ctx) {
    for (auto identifier : identifiers) {
        if (ctx.getVariable(identifier->value, false) != nullptr)
            throw Interpreter::RedeclarationError(token, ctx, identifier->value);

        if (ctx.isIdentifierType(*identifier))
            throw Interpreter::RuntimeError(token, ctx, "Redefinition of type '" + identifier->value + "' as variable");

        Interpreter::DataType dataType = ctx.getType(type);
        if (dataType == Interpreter::DataType::NONE)
            throw Interpreter::NotDefinedError(token, ctx, "Type '" + type.value + "'");

        ctx.addVariable(new Interpreter::Variable(identifier->value, dataType, false, &ctx));
    }

    return std::make_unique<NodeResult>(nullptr, Interpreter::DataType::NONE);
}


ConstDeclareNode::ConstDeclareNode(const Token &token, Node &node, const Token &identifier)
    : UnaryNode(token, node), identifier(identifier)
{}

std::unique_ptr<NodeResult> ConstDeclareNode::evaluate(Interpreter::Context &ctx) {
    auto value = node.evaluate(ctx);

    if (ctx.getVariable(identifier.value, false) != nullptr)
        throw Interpreter::RedeclarationError(token, ctx, identifier.value);

    ctx.addVariable(new Interpreter::Variable(identifier.value, value->type, true, &ctx, value->data.get()));

    return std::make_unique<NodeResult>(nullptr, Interpreter::DataType::NONE);
}


AssignNode::AssignNode(const Token &token, Node &node, std::unique_ptr<AbstractVariableResolver> &&resolver)
    : UnaryNode(token, node), resolver(std::move(resolver))
{}

void AssignNode::assignArray(Interpreter::Context &ctx, const Interpreter::ArrayDirectAccessError &e) {
    AccessNode *accsNode = dynamic_cast<AccessNode*>(&node);
    if (accsNode == nullptr) throw e;
    auto array = static_cast<Interpreter::Array*>(&accsNode->resolver->resolve(ctx));
    
    Interpreter::DataHolder &holder = resolver->resolve(ctx);
    if (!holder.isArray()) throw e;
    
    Interpreter::Array *arr = static_cast<Interpreter::Array*>(&holder);
    if (arr->type != array->type)
        throw Interpreter::RuntimeError(token, ctx, "Cannot assign arrays of different data type");
    if (arr->dimensions != array->dimensions)
        throw Interpreter::RuntimeError(token, ctx, "Cannot assign arrays of different dimensions");

    arr->copyData(*array);
}

std::unique_ptr<NodeResult> AssignNode::evaluate(Interpreter::Context &ctx) {
    std::unique_ptr<NodeResult> valueRes;
    try {
        valueRes = node.evaluate(ctx);
    } catch (Interpreter::ArrayDirectAccessError &e) {
        if (&e.context != &ctx) throw e;
        assignArray(ctx, e);
        return std::make_unique<NodeResult>(nullptr, Interpreter::DataType::NONE);
    }

    Interpreter::Variable *var;
    try {
        Interpreter::DataHolder &holder = resolver->resolve(ctx);
        if (holder.isArray())
            throw Interpreter::ArrayDirectAccessError(token, ctx);

        var = static_cast<Interpreter::Variable*>(&holder);
    } catch (Interpreter::NotDefinedError &e) {
        const SimpleVariableSource *simpleSource = dynamic_cast<const SimpleVariableSource*>(resolver.get());
        if (simpleSource == nullptr) throw e;
        if (ctx.isIdentifierType(simpleSource->getToken())) throw e;

        var = new Interpreter::Variable(simpleSource->getName(), valueRes->type, false, &ctx);
        ctx.addVariable(var);
    }

    if (var->isConstant)
        throw Interpreter::ConstAssignError(token, ctx, var->name);

    valueRes->implicitCast(var->type);
    if (var->type != valueRes->type)
        throw Interpreter::InvalidUsageError(token, ctx, "assignment operator: incompatible data types");

    switch (var->type.type) {
        case Interpreter::DataType::INTEGER:
            var->get<Interpreter::Integer>() = valueRes->get<Interpreter::Integer>();
            break;
        case Interpreter::DataType::REAL:
            var->get<Interpreter::Real>() = valueRes->get<Interpreter::Real>();
            break;
        case Interpreter::DataType::BOOLEAN:
            var->get<Interpreter::Boolean>() = valueRes->get<Interpreter::Boolean>();
            break;
        case Interpreter::DataType::CHAR:
            var->get<Interpreter::Char>() = valueRes->get<Interpreter::Char>();
            break;
        case Interpreter::DataType::STRING:
            var->get<Interpreter::String>() = valueRes->get<Interpreter::String>();
            break;
        case Interpreter::DataType::DATE:
            var->get<Interpreter::Date>() = valueRes->get<Interpreter::Date>();
            break;
        case Interpreter::DataType::ENUM:
            var->get<Interpreter::Enum>() = valueRes->get<Interpreter::Enum>();
            break;
        case Interpreter::DataType::POINTER:
            var->get<Interpreter::Pointer>() = valueRes->get<Interpreter::Pointer>();
            break;
        case Interpreter::DataType::COMPOSITE:
            var->get<Interpreter::Composite>() = valueRes->get<Interpreter::Composite>();
            break;
        case Interpreter::DataType::NONE:
            std::abort();
    }

    return std::make_unique<NodeResult>(nullptr, Interpreter::DataType::NONE);
}


AccessNode::AccessNode(const Token &token, std::unique_ptr<AbstractVariableResolver> &&resolver)
    : Node(token), resolver(std::move(resolver)) {}

std::unique_ptr<NodeResult> AccessNode::evaluate(Interpreter::Context &ctx) {
    Interpreter::DataHolder *holder;
    try {
        holder = &resolver->resolve(ctx);
    } catch (Interpreter::NotDefinedError &e) {
        auto def = ctx.getEnumElement(token.value);
        if (def != nullptr)
            return std::make_unique<NodeResult>(def, Interpreter::DataType(Interpreter::DataType::ENUM, &def->definitionName));
        throw e;
    }

    if (holder->isArray())
        throw Interpreter::ArrayDirectAccessError(token, ctx);
    
    Interpreter::Variable &var = *static_cast<Interpreter::Variable*>(holder);

    Interpreter::Value *data;
    switch (var.type.type) {
        case Interpreter::DataType::INTEGER:
            data = new Interpreter::Integer(var.get<Interpreter::Integer>());
            break;
        case Interpreter::DataType::REAL:
            data = new Interpreter::Real(var.get<Interpreter::Real>());
            break;
        case Interpreter::DataType::BOOLEAN:
            data = new Interpreter::Boolean(var.get<Interpreter::Boolean>());
            break;
        case Interpreter::DataType::CHAR:
            data = new Interpreter::Char(var.get<Interpreter::Char>());
            break;
        case Interpreter::DataType::STRING:
            data = new Interpreter::String(var.get<Interpreter::String>());
            break;
        case Interpreter::DataType::DATE:
            data = new Interpreter::Date(var.get<Interpreter::Date>());
            break;
        case Interpreter::DataType::ENUM:
            data = new Interpreter::Enum(var.get<Interpreter::Enum>());
            break;
        case Interpreter::DataType::POINTER:
            data = new Interpreter::Pointer(var.get<Interpreter::Pointer>());
            break;
        case Interpreter::DataType::COMPOSITE:
            data = new Interpreter::Composite(var.get<Interpreter::Composite>());
            break;
        case Interpreter::DataType::NONE:
            std::abort();
    }

    return std::make_unique<NodeResult>(data, var.type);
}

const AbstractVariableResolver &AccessNode::getResolver() const {
    return *resolver;
}
