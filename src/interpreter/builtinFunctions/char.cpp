#include "pch.h"
#include <ctype.h>

#include "interpreter/variable.h"
#include "interpreter/scope/context.h"
#include "interpreter/builtinFunctions/functions.h"

Interpreter::BuiltinFnLCase::BuiltinFnLCase()
    : Function("LCASE", Interpreter::DataType::CHAR)
{
    parameters.emplace_back("Char", Interpreter::DataType::CHAR, false);
}

void Interpreter::BuiltinFnLCase::run(Interpreter::Context &ctx) {
    Interpreter::Variable *ch = ctx.getVariable("Char");
    if (ch == nullptr || ch->type != Interpreter::DataType::CHAR) std::abort();

    auto ret = std::make_unique<Interpreter::Char>();
    ret->value = (char) std::tolower(ch->get<Interpreter::Char>().value);

    ctx.returnValue = std::make_unique<NodeResult>(std::move(ret), Interpreter::DataType::CHAR);
}


Interpreter::BuiltinFnUCase::BuiltinFnUCase()
    : Function("UCASE", Interpreter::DataType::CHAR)
{
    parameters.emplace_back("Char", Interpreter::DataType::CHAR, false);
}

void Interpreter::BuiltinFnUCase::run(Interpreter::Context &ctx) {
    Interpreter::Variable *ch = ctx.getVariable("Char");
    if (ch == nullptr || ch->type != Interpreter::DataType::CHAR) std::abort();

    auto ret = std::make_unique<Interpreter::Char>();
    ret->value = (char) std::toupper(ch->get<Interpreter::Char>().value);

    ctx.returnValue = std::make_unique<NodeResult>(std::move(ret), Interpreter::DataType::CHAR);
}


Interpreter::BuiltinFnASC::BuiltinFnASC()
    : Function("ASC", Interpreter::DataType::INTEGER)
{
    parameters.emplace_back("Char", Interpreter::DataType::CHAR, false);
}

void Interpreter::BuiltinFnASC::run(Interpreter::Context &ctx) {
    Interpreter::Variable *ch = ctx.getVariable("Char");
    if (ch == nullptr || ch->type != Interpreter::DataType::CHAR) std::abort();

    auto ret = std::make_unique<Interpreter::Integer>();
    ret->value = (int_t) ch->get<Interpreter::Char>().value;

    ctx.returnValue = std::make_unique<NodeResult>(std::move(ret), Interpreter::DataType::INTEGER);
}


Interpreter::BuiltinFnCHR::BuiltinFnCHR()
    : Function("CHR", Interpreter::DataType::CHAR)
{
    parameters.emplace_back("x", Interpreter::DataType::INTEGER, false);
}

void Interpreter::BuiltinFnCHR::run(Interpreter::Context &ctx) {
    Interpreter::Variable *x = ctx.getVariable("x");
    if (x == nullptr || x->type != Interpreter::DataType::INTEGER) std::abort();

    auto ret = std::make_unique<Interpreter::Char>();
    ret->value = (char) x->get<Interpreter::Integer>().value;

    ctx.returnValue = std::make_unique<NodeResult>(std::move(ret), Interpreter::DataType::CHAR);
}
