#include "pch.h"

#include "interpreter/scope/block.h"
#include "interpreter/scope/context.h"
#include "interpreter/procedure.h"

using namespace Interpreter;

Parameter::Parameter(const std::string &name, Interpreter::DataType type, bool byRef)
    : name(name), type(type), byRef(byRef)
{}

Procedure::Procedure(
		const std::string &name,
		std::vector<Parameter> &&parameters,
		Interpreter::Block *block
)
: name(name),
    parameters(std::move(parameters)),
    block(block)
{}

Procedure::Procedure(const std::string &name)
    : name(name), parameters(), block(nullptr)
{}

std::vector<Interpreter::DataType> Procedure::getTypes() const {
    std::vector<Interpreter::DataType> types;
    types.reserve(parameters.size());
    for (auto p : parameters) types.push_back(p.type);
    return types;
}

void Procedure::run(Interpreter::Context &ctx) {
    block->run(ctx);
}

Function::Function(
		const std::string &name,
		std::vector<Parameter> &&parameters,
		Interpreter::Block *block,
		Interpreter::DataType returnType,
		const Token *defToken
)
: Procedure(name, std::move(parameters), block),
    returnType(returnType),
    defToken(defToken)
{}

Function::Function(const std::string &name, Interpreter::DataType returnType)
    : Procedure(name),
    returnType(returnType),
    defToken(nullptr)
{}
