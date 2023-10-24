#include "pch.h"
#include <math.h>

#include "interpreter/variable.h"
#include "interpreter/scope/context.h"
#include "interpreter/builtinFunctions/functions.h"


Interpreter::BuiltinFnRand::BuiltinFnRand()
    : Function("RAND", Interpreter::DataType::REAL)
{
    parameters.emplace_back("x", Interpreter::DataType::INTEGER, false);
}

void Interpreter::BuiltinFnRand::run(Interpreter::Context &ctx) {
    Interpreter::Variable *x = ctx.getVariable("x");
    if (x == nullptr || x->type != Interpreter::DataType::INTEGER) std::abort();

    auto ret = std::make_unique<Interpreter::Real>(rand() % x->get<Interpreter::Integer>().value);
    ret->value += rand() / (real_t) RAND_MAX;

    ctx.returnValue = std::make_unique<NodeResult>(std::move(ret), Interpreter::DataType::REAL);
}


Interpreter::BuiltinFnInt::BuiltinFnInt()
    : Function("INT", Interpreter::DataType::INTEGER)
{
    parameters.emplace_back("x", Interpreter::DataType::REAL, false);
}

void Interpreter::BuiltinFnInt::run(Interpreter::Context &ctx) {
    Interpreter::Variable *x = ctx.getVariable("x");
    if (x == nullptr || x->type != Interpreter::DataType::REAL) std::abort();

    auto ret = std::make_unique<Interpreter::Integer>((int_t) floor(x->get<Interpreter::Real>().value));

    ctx.returnValue = std::make_unique<NodeResult>(std::move(ret), Interpreter::DataType::INTEGER);
}
