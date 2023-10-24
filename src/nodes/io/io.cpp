#include "pch.h"
#include <iostream>

#include "interpreter/error.h"
#include "nodes/io/io.h"

OutputNode::OutputNode(const Token &token, std::vector<Node*> &&nodes)
    : Node(token), nodes(std::move(nodes))
{}

std::unique_ptr<NodeResult> OutputNode::evaluate(Interpreter::Context &ctx) {
    for (Node *node : nodes) {
        auto result = node->evaluate(ctx);

        switch (result->type.type) {
            case Interpreter::DataType::INTEGER:
                std::cout << result->get<Interpreter::Integer>();
                break;
            case Interpreter::DataType::REAL: {
                Interpreter::real_t value = result->get<Interpreter::Real>();
                std::cout << value;

                double integerValue;
                if (std::modf(value, &integerValue) == 0.0) std::cout << ".0";
                break;
            } case Interpreter::DataType::BOOLEAN:
                std::cout << (result->get<Interpreter::Boolean>() ? "TRUE" : "FALSE");
                break;
            case Interpreter::DataType::CHAR:
                std::cout << result->get<Interpreter::Char>();
                break;
            case Interpreter::DataType::STRING:
                std::cout << result->get<Interpreter::String>().value;
                break;
            case Interpreter::DataType::DATE: {
                auto str = result->get<Interpreter::Date>().toString();
                std::cout << str->value;
                break;
            } case Interpreter::DataType::ENUM:
                std::cout << result->get<Interpreter::Enum>().getString(ctx);
                break;
            case Interpreter::DataType::POINTER:
                std::cout << result->get<Interpreter::Pointer>().definitionName << " object";
                break;
            case Interpreter::DataType::COMPOSITE:
                std::cout << result->get<Interpreter::Composite>().definitionName << " object";
                break;
            case Interpreter::DataType::NONE:
                std::cout.flush();
                throw Interpreter::RuntimeError(node->getToken(), ctx, "Expected a value for OUTPUT/PRINT");
        }
    }
    std::cout << std::endl;

    return std::make_unique<NodeResult>(nullptr, Interpreter::DataType::NONE);
}


InputNode::InputNode(const Token &token, std::unique_ptr<AbstractVariableResolver> &&resolver)
    : Node(token), resolver(std::move(resolver))
{}

std::unique_ptr<NodeResult> InputNode::evaluate(Interpreter::Context &ctx) {
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

        var = new Interpreter::Variable(simpleSource->getName(), Interpreter::DataType::STRING, false, &ctx);
        ctx.addVariable(var);
    }

    Interpreter::String inputStr;
    std::getline(std::cin, inputStr.value);

    switch (var->type.type) {
        case Interpreter::DataType::INTEGER:
            var->get<Interpreter::Integer>() = inputStr.toInteger()->value;
            break;
        case Interpreter::DataType::REAL:
            var->get<Interpreter::Real>() = inputStr.toReal()->value;
            break;
        case Interpreter::DataType::BOOLEAN:
            var->get<Interpreter::Boolean>() = (inputStr.value == "TRUE");
            break;
        case Interpreter::DataType::CHAR:
            var->get<Interpreter::Char>() = inputStr.value.front();
            break;
        case Interpreter::DataType::STRING:
            var->get<Interpreter::String>().value = std::move(inputStr.value);
            break;
        case Interpreter::DataType::DATE:
        case Interpreter::DataType::ENUM:
        case Interpreter::DataType::POINTER:
        case Interpreter::DataType::COMPOSITE:
            throw Interpreter::RuntimeError(token, ctx, "Cannot input non-primitive types");
        case Interpreter::DataType::NONE:
            std::abort();
    }

    return std::make_unique<NodeResult>(nullptr, Interpreter::DataType::NONE);
}
