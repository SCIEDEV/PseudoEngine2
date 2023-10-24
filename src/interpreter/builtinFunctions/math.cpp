#include "pch.h"
#include <math.h>

#include "interpreter/builtinFunctions/functions.h"
#include "interpreter/scope/context.h"

Interpreter::BuiltinFnPow::BuiltinFnPow()
    : Function("POW", Interpreter::DataType::REAL)
{
    parameters.emplace_back("x", Interpreter::DataType::REAL, false);
    parameters.emplace_back("y", Interpreter::DataType::REAL, false);
}

void Interpreter::BuiltinFnPow::run(Interpreter::Context &ctx) {
    Interpreter::Variable *x = ctx.getVariable("x");
    if (x == nullptr || x->type != Interpreter::DataType::REAL) std::abort();

    Interpreter::Variable *y = ctx.getVariable("y");
    if (y == nullptr || y->type != Interpreter::DataType::REAL) std::abort();

    Interpreter::Real xVal = x->get<Interpreter::Real>();
    Interpreter::Real yVal = y->get<Interpreter::Real>();
    auto ret = std::make_unique<Interpreter::Real>((Interpreter::real_t) pow(xVal, yVal));

    ctx.returnValue = std::make_unique<NodeResult>(std::move(ret), Interpreter::DataType::REAL);
}

Interpreter::BuiltinFnExp::BuiltinFnExp()
    : Function("EXP", Interpreter::DataType::REAL)
{
    parameters.emplace_back("x", Interpreter::DataType::REAL, false);
}

void Interpreter::BuiltinFnExp::run(Interpreter::Context &ctx) {
    Interpreter::Variable *x = ctx.getVariable("x");
    if (x == nullptr || x->type != Interpreter::DataType::REAL) std::abort();

    Interpreter::Real xVal = x->get<Interpreter::Real>();
    auto ret = std::make_unique<Interpreter::Real>((Interpreter::real_t) exp(xVal));

    ctx.returnValue = std::make_unique<NodeResult>(std::move(ret), Interpreter::DataType::REAL);
}

Interpreter::BuiltinFnSin::BuiltinFnSin()
    : Function("SIN", Interpreter::DataType::REAL)
{
    parameters.emplace_back("x", Interpreter::DataType::REAL, false);
}

void Interpreter::BuiltinFnSin::run(Interpreter::Context &ctx) {
    Interpreter::Variable *x = ctx.getVariable("x");
    if (x == nullptr || x->type != Interpreter::DataType::REAL) std::abort();

    Interpreter::Real xVal = x->get<Interpreter::Real>();
    auto ret = std::make_unique<Interpreter::Real>((Interpreter::real_t) sin(xVal));

    ctx.returnValue = std::make_unique<NodeResult>(std::move(ret), Interpreter::DataType::REAL);
}

Interpreter::BuiltinFnCos::BuiltinFnCos()
    : Function("COS", Interpreter::DataType::REAL)
{
    parameters.emplace_back("x", Interpreter::DataType::REAL, false);
}

void Interpreter::BuiltinFnCos::run(Interpreter::Context &ctx) {
    Interpreter::Variable *x = ctx.getVariable("x");
    if (x == nullptr || x->type != Interpreter::DataType::REAL) std::abort();

    Interpreter::Real xVal = x->get<Interpreter::Real>();
    auto ret = std::make_unique<Interpreter::Real>((Interpreter::real_t) cos(xVal));

    ctx.returnValue = std::make_unique<NodeResult>(std::move(ret), Interpreter::DataType::REAL);
}

Interpreter::BuiltinFnTan::BuiltinFnTan()
    : Function("TAN", Interpreter::DataType::REAL)
{
    parameters.emplace_back("x", Interpreter::DataType::REAL, false);
}

void Interpreter::BuiltinFnTan::run(Interpreter::Context &ctx) {
    Interpreter::Variable *x = ctx.getVariable("x");
    if (x == nullptr || x->type != Interpreter::DataType::REAL) std::abort();

    Interpreter::Real xVal = x->get<Interpreter::Real>();
    auto ret = std::make_unique<Interpreter::Real>((Interpreter::real_t) tan(xVal));

    ctx.returnValue = std::make_unique<NodeResult>(std::move(ret), Interpreter::DataType::REAL);
}

Interpreter::BuiltinFnASin::BuiltinFnASin()
    : Function("ASIN", Interpreter::DataType::REAL)
{
    parameters.emplace_back("x", Interpreter::DataType::REAL, false);
}

void Interpreter::BuiltinFnASin::run(Interpreter::Context &ctx) {
    Interpreter::Variable *x = ctx.getVariable("x");
    if (x == nullptr || x->type != Interpreter::DataType::REAL) std::abort();

    Interpreter::Real xVal = x->get<Interpreter::Real>();
    auto ret = std::make_unique<Interpreter::Real>((Interpreter::real_t) asin(xVal));

    ctx.returnValue = std::make_unique<NodeResult>(std::move(ret), Interpreter::DataType::REAL);
}

Interpreter::BuiltinFnACos::BuiltinFnACos()
    : Function("ACOS", Interpreter::DataType::REAL)
{
    parameters.emplace_back("x", Interpreter::DataType::REAL, false);
}

void Interpreter::BuiltinFnACos::run(Interpreter::Context &ctx) {
    Interpreter::Variable *x = ctx.getVariable("x");
    if (x == nullptr || x->type != Interpreter::DataType::REAL) std::abort();

    Interpreter::Real xVal = x->get<Interpreter::Real>();
    auto ret = std::make_unique<Interpreter::Real>((Interpreter::real_t) acos(xVal));

    ctx.returnValue = std::make_unique<NodeResult>(std::move(ret), Interpreter::DataType::REAL);
}

Interpreter::BuiltinFnATan::BuiltinFnATan()
    : Function("ATAN", Interpreter::DataType::REAL)
{
    parameters.emplace_back("x", Interpreter::DataType::REAL, false);
}

void Interpreter::BuiltinFnATan::run(Interpreter::Context &ctx) {
    Interpreter::Variable *x = ctx.getVariable("x");
    if (x == nullptr || x->type != Interpreter::DataType::REAL) std::abort();

    Interpreter::Real xVal = x->get<Interpreter::Real>();
    auto ret = std::make_unique<Interpreter::Real>((Interpreter::real_t) atan(xVal));

    ctx.returnValue = std::make_unique<NodeResult>(std::move(ret), Interpreter::DataType::REAL);
}

Interpreter::BuiltinFnATan2::BuiltinFnATan2()
    : Function("ATAN2", Interpreter::DataType::REAL)
{
    parameters.emplace_back("y", Interpreter::DataType::REAL, false);
    parameters.emplace_back("x", Interpreter::DataType::REAL, false);
}

void Interpreter::BuiltinFnATan2::run(Interpreter::Context &ctx) {
    Interpreter::Variable *y = ctx.getVariable("y");
    if (y == nullptr || y->type != Interpreter::DataType::REAL) std::abort();

    Interpreter::Variable *x = ctx.getVariable("x");
    if (x == nullptr || x->type != Interpreter::DataType::REAL) std::abort();

    Interpreter::Real yVal = y->get<Interpreter::Real>();
    Interpreter::Real xVal = x->get<Interpreter::Real>();
    auto ret = std::make_unique<Interpreter::Real>((Interpreter::real_t) atan2(yVal, xVal));

    ctx.returnValue = std::make_unique<NodeResult>(std::move(ret), Interpreter::DataType::REAL);
}

Interpreter::BuiltinFnSqrt::BuiltinFnSqrt()
    : Function("SQRT", Interpreter::DataType::REAL)
{
    parameters.emplace_back("x", Interpreter::DataType::REAL, false);
}

void Interpreter::BuiltinFnSqrt::run(Interpreter::Context &ctx) {
    Interpreter::Variable *x = ctx.getVariable("x");
    if (x == nullptr || x->type != Interpreter::DataType::REAL) std::abort();

    Interpreter::Real xVal = x->get<Interpreter::Real>();
    auto ret = std::make_unique<Interpreter::Real>((Interpreter::real_t) sqrt(xVal));

    ctx.returnValue = std::make_unique<NodeResult>(std::move(ret), Interpreter::DataType::REAL);
}

Interpreter::BuiltinFnLog::BuiltinFnLog()
    : Function("LOG", Interpreter::DataType::REAL)
{
    parameters.emplace_back("x", Interpreter::DataType::REAL, false);
}

void Interpreter::BuiltinFnLog::run(Interpreter::Context &ctx) {
    Interpreter::Variable *x = ctx.getVariable("x");
    if (x == nullptr || x->type != Interpreter::DataType::REAL) std::abort();

    Interpreter::Real xVal = x->get<Interpreter::Real>();
    auto ret = std::make_unique<Interpreter::Real>((Interpreter::real_t) log10(xVal));

    ctx.returnValue = std::make_unique<NodeResult>(std::move(ret), Interpreter::DataType::REAL);
}

Interpreter::BuiltinFnLn::BuiltinFnLn()
    : Function("LN", Interpreter::DataType::REAL)
{
    parameters.emplace_back("x", Interpreter::DataType::REAL, false);
}

void Interpreter::BuiltinFnLn::run(Interpreter::Context &ctx) {
    Interpreter::Variable *x = ctx.getVariable("x");
    if (x == nullptr || x->type != Interpreter::DataType::REAL) std::abort();

    Interpreter::Real xVal = x->get<Interpreter::Real>();
    auto ret = std::make_unique<Interpreter::Real>((Interpreter::real_t) log(xVal));

    ctx.returnValue = std::make_unique<NodeResult>(std::move(ret), Interpreter::DataType::REAL);
}
