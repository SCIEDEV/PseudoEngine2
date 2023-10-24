#include "pch.h"

#include "interpreter/error.h"
#include "interpreter/scope/block.h"
#include "nodes/variable/variable.h"
#include "nodes/variable/array.h"
#include "nodes/functions/function.h"

FunctionNode::FunctionNode(
		const Token &token,
		const std::string &functionName,
		std::vector<std::string> &&parameterNames,
		std::vector<const Token*> &&parameterTypes,
		std::vector<bool> &&parameterPassTypes,
		Interpreter::Block &block,
		const Token &returnType
)
: Node(token),
    functionName(functionName),
    parameterNames(std::move(parameterNames)),
    parameterTypes(std::move(parameterTypes)),
    parameterPassTypes(std::move(parameterPassTypes)),
    block(block),
    returnType(returnType)
{}

std::unique_ptr<NodeResult> FunctionNode::evaluate(Interpreter::Context &ctx) {
    if (ctx.getFunction(functionName) != nullptr)
        throw Interpreter::RedefinitionError(token, ctx, functionName);

    Interpreter::DataType returnDataType = ctx.getType(returnType);
    if (returnDataType == Interpreter::DataType::NONE)
        throw Interpreter::NotDefinedError(returnType, ctx, "Type '" + returnType.value + "'");

    size_t parametersSize = parameterNames.size();
    std::vector<Interpreter::Parameter> parameters;
    parameters.reserve(parametersSize);
    for (size_t i = 0; i < parametersSize; i++) {
        const Token *typeToken = parameterTypes[i];
        Interpreter::DataType type = ctx.getType(*typeToken);
        if (type == Interpreter::DataType::NONE)
            throw Interpreter::NotDefinedError(*typeToken, ctx, "Type '" + typeToken->value + "'");
        parameters.emplace_back(parameterNames[i], type, parameterPassTypes[i]);
    }

    auto function = std::make_unique<Interpreter::Function>(functionName, std::move(parameters), &block, returnDataType, &token);
    ctx.addFunction(std::move(function));

    return std::make_unique<NodeResult>(nullptr, Interpreter::DataType::NONE);
}


FunctionCallNode::FunctionCallNode(const Token &token, std::vector<Node*> &&args)
    : Node(token), functionName(token.value), args(std::move(args))
{}

std::unique_ptr<NodeResult> FunctionCallNode::evaluate(Interpreter::Context &ctx) {
    Interpreter::Function *function = ctx.getFunction(functionName);

    if (function == nullptr)
        throw Interpreter::NotDefinedError(token, ctx, "Function '" + functionName + "'");

    std::vector<std::unique_ptr<NodeResult>> argResults;
    std::vector<Interpreter::DataType> argTypes;
    argResults.reserve(args.size());
    argTypes.reserve(args.size());

    for (size_t i = 0; i < args.size(); i++) {
        argResults.push_back(args[i]->evaluate(ctx));
        argTypes.push_back(argResults[i]->type);
    }

    size_t nArgs = function->parameters.size();
    if (args.size() != nArgs)
        throw Interpreter::InvalidArgsError(token, ctx, function->getTypes(), std::move(argTypes));

    auto functionCtx = std::make_unique<Interpreter::Context>(&ctx, functionName, true, function->returnType);
    ctx.switchToken = &token;

    for (size_t i = 0; i < args.size(); i++) {
        auto &argRes = argResults[i];
        const Interpreter::Parameter &parameter = function->parameters[i];

        if (!parameter.byRef) argRes->implicitCast(parameter.type);
        if (parameter.type != argRes->type) {
            throw Interpreter::InvalidArgsError(token, ctx, function->getTypes(), std::move(argTypes));
        }

        Interpreter::Variable *var;
        if (parameter.byRef) {
            AccessNode *accsNode = dynamic_cast<AccessNode*>(args[i]);
            if (!accsNode) {
                throw Interpreter::RuntimeError(token, ctx, "Only variables can be used as arguements when passing by reference");
            }

            auto &holder = accsNode->getResolver().resolve(ctx);
            if (holder.isArray())
                throw Interpreter::ArrayDirectAccessError(token, ctx);

            Interpreter::Variable &original = *static_cast<Interpreter::Variable*>(&holder);
            var = original.createReference(parameter.name);
        } else {
            var = new Interpreter::Variable(parameter.name, argRes->type, false, functionCtx.get());

            switch (var->type.type) {
                case Interpreter::DataType::INTEGER:
                    var->get<Interpreter::Integer>() = argRes->get<Interpreter::Integer>();
                    break;
                case Interpreter::DataType::REAL:
                    var->get<Interpreter::Real>() = argRes->get<Interpreter::Real>();
                    break;
                case Interpreter::DataType::BOOLEAN:
                    var->get<Interpreter::Boolean>() = argRes->get<Interpreter::Boolean>();
                    break;
                case Interpreter::DataType::CHAR:
                    var->get<Interpreter::Char>() = argRes->get<Interpreter::Char>();
                    break;
                case Interpreter::DataType::STRING:
                    var->get<Interpreter::String>() = argRes->get<Interpreter::String>();
                    break;
                case Interpreter::DataType::DATE:
                    var->get<Interpreter::Date>() = argRes->get<Interpreter::Date>();
                    break;
                case Interpreter::DataType::ENUM:
                    var->get<Interpreter::Enum>() = argRes->get<Interpreter::Enum>();
                    break;
                case Interpreter::DataType::POINTER:
                    var->get<Interpreter::Pointer>() = argRes->get<Interpreter::Pointer>();
                    break;
                case Interpreter::DataType::COMPOSITE:
                    var->get<Interpreter::Composite>() = argRes->get<Interpreter::Composite>();
                    break;
                case Interpreter::DataType::NONE:
                    std::abort();
            }
        }

        functionCtx->addVariable(var);
    }

    try {
        function->run(*functionCtx);
    } catch (ReturnErrSignal&) {}

    if (!functionCtx->returnValue)
        throw Interpreter::RuntimeError(*(function->defToken), *functionCtx, "Missing RETURN statement");

    ctx.switchToken = nullptr;

    return std::move(functionCtx->returnValue);
}

std::unique_ptr<NodeResult> ReturnNode::evaluate(Interpreter::Context &ctx) {
    if (!ctx.isFunctionCtx) 
        throw Interpreter::InvalidUsageError(token, ctx, "RETURN statement");

    ctx.returnValue = node.evaluate(ctx);
    ctx.returnValue->implicitCast(ctx.returnType);

    if (ctx.returnValue->type != ctx.returnType)
        throw Interpreter::RuntimeError(token, ctx, "Invalid return type");

    throw ReturnErrSignal();
}
