#include "pch.h"

#include "interpreter/error.h"
#include "interpreter/scope/block.h"
#include "nodes/variable/variable.h"
#include "nodes/variable/array.h"
#include "nodes/functions/procedure.h"

ProcedureNode::ProcedureNode(
		const Token &token,
		const std::string &procedureName,
		std::vector<std::string> &&parameterNames,
		std::vector<const Token*> &&parameterTypes,
		std::vector<bool> &&parameterPassTypes,
		Interpreter::Block &block
)
: Node(token),
    procedureName(procedureName),
    parameterNames(std::move(parameterNames)),
    parameterTypes(std::move(parameterTypes)),
    parameterPassTypes(std::move(parameterPassTypes)),
    block(block)
{}

std::unique_ptr<NodeResult> ProcedureNode::evaluate(Interpreter::Context &ctx) {
    if (ctx.getProcedure(procedureName) != nullptr)
        throw Interpreter::RedefinitionError(token, ctx, procedureName);

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

    auto procedure = std::make_unique<Interpreter::Procedure>(procedureName, std::move(parameters), &block);
    ctx.addProcedure(std::move(procedure));

    return std::make_unique<NodeResult>(nullptr, Interpreter::DataType::NONE);
}


CallNode::CallNode(const Token &token, const std::string &procedureName, std::vector<Node*> &&args)
    : Node(token), procedureName(procedureName), args(std::move(args))
{}

std::unique_ptr<NodeResult> CallNode::evaluate(Interpreter::Context &ctx) {
    Interpreter::Procedure *procedure = ctx.getProcedure(procedureName);

    if (procedure == nullptr)
        throw Interpreter::NotDefinedError(token, ctx, "Procedure '" + procedureName + "'");

    std::vector<std::unique_ptr<NodeResult>> argResults;
    std::vector<Interpreter::DataType> argTypes;
    argResults.reserve(args.size());
    argTypes.reserve(args.size());

    for (size_t i = 0; i < args.size(); i++) {
        argResults.push_back(args[i]->evaluate(ctx));
        argTypes.push_back(argResults[i]->type);
    }

    size_t nArgs = procedure->parameters.size();
    if (args.size() != nArgs)
        throw Interpreter::InvalidArgsError(token, ctx, procedure->getTypes(), std::move(argTypes));

    auto procedureCtx = std::make_unique<Interpreter::Context>(&ctx, procedureName);
    ctx.switchToken = &token;

    for (size_t i = 0; i < args.size(); i++) {
        auto &argRes = argResults[i];
        const Interpreter::Parameter &parameter = procedure->parameters[i];

        if (!parameter.byRef) argRes->implicitCast(parameter.type);
        if (parameter.type != argRes->type)
            throw Interpreter::InvalidArgsError(token, ctx, procedure->getTypes(), std::move(argTypes));

        Interpreter::Variable *var;
        if (parameter.byRef) {
            AccessNode *accsNode = dynamic_cast<AccessNode*>(args[i]);
            if (!accsNode)
                throw Interpreter::RuntimeError(token, ctx, "Only variables and array elements can be used as arguements when passing by reference");

            auto &holder = accsNode->getResolver().resolve(ctx);
            if (holder.isArray())
                throw Interpreter::ArrayDirectAccessError(token, ctx);

            Interpreter::Variable &original = *static_cast<Interpreter::Variable*>(&holder);
            var = original.createReference(parameter.name);
        } else {
            var = new Interpreter::Variable(parameter.name, argRes->type, false, procedureCtx.get());

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

        procedureCtx->addVariable(var);
    }

    procedure->run(*procedureCtx);
    ctx.switchToken = nullptr;

    return std::make_unique<NodeResult>(nullptr, Interpreter::DataType::NONE);
}
