#include "pch.h"

#include "interpreter/error.h"
#include "interpreter/scope/context.h"
#include "interpreter/builtinFunctions/functions.h"

using namespace std::chrono;

Interpreter::BuiltinFnDAY::BuiltinFnDAY()
    : Function("DAY", Interpreter::DataType::INTEGER)
{
    parameters.emplace_back("Date", Interpreter::DataType::DATE, false);
}

void Interpreter::BuiltinFnDAY::run(Interpreter::Context &ctx) {
    Interpreter::Variable *var = ctx.getVariable("Date");
    if (var == nullptr || var->type != Interpreter::DataType::DATE) std::abort();

    int_t day = static_cast<unsigned int>(var->get<Interpreter::Date>().date.day());
    auto ret = std::make_unique<Interpreter::Integer>(day);

    ctx.returnValue = std::make_unique<NodeResult>(std::move(ret), Interpreter::DataType::INTEGER);
}

Interpreter::BuiltinFnMONTH::BuiltinFnMONTH()
    : Function("MONTH", Interpreter::DataType::INTEGER)
{
    parameters.emplace_back("Date", Interpreter::DataType::DATE, false);
}

void Interpreter::BuiltinFnMONTH::run(Interpreter::Context &ctx) {
    Interpreter::Variable *var = ctx.getVariable("Date");
    if (var == nullptr || var->type != Interpreter::DataType::DATE) std::abort();

    int_t day = static_cast<unsigned int>(var->get<Interpreter::Date>().date.month());
    auto ret = std::make_unique<Interpreter::Integer>(day);

    ctx.returnValue = std::make_unique<NodeResult>(std::move(ret), Interpreter::DataType::INTEGER);
}

Interpreter::BuiltinFnYEAR::BuiltinFnYEAR()
    : Function("YEAR", Interpreter::DataType::INTEGER)
{
    parameters.emplace_back("Date", Interpreter::DataType::DATE, false);
}

void Interpreter::BuiltinFnYEAR::run(Interpreter::Context &ctx) {
    Interpreter::Variable *var = ctx.getVariable("Date");
    if (var == nullptr || var->type != Interpreter::DataType::DATE) std::abort();

    int_t day = static_cast<int>(var->get<Interpreter::Date>().date.year());
    auto ret = std::make_unique<Interpreter::Integer>(day);

    ctx.returnValue = std::make_unique<NodeResult>(std::move(ret), Interpreter::DataType::INTEGER);
}

Interpreter::BuiltinFnDAYINDEX::BuiltinFnDAYINDEX()
    : Function("DAYINDEX", Interpreter::DataType::INTEGER)
{
    parameters.emplace_back("Date", Interpreter::DataType::DATE, false);
}

void Interpreter::BuiltinFnDAYINDEX::run(Interpreter::Context &ctx) {
    Interpreter::Variable *var = ctx.getVariable("Date");
    if (var == nullptr || var->type != Interpreter::DataType::DATE) std::abort();

    auto date = var->get<Interpreter::Date>().date;
    weekday weekday(date);
    int_t day = weekday.c_encoding() + 1;
    auto ret = std::make_unique<Interpreter::Integer>(day);

    ctx.returnValue = std::make_unique<NodeResult>(std::move(ret), Interpreter::DataType::INTEGER);
}

Interpreter::BuiltinFnSETDATE::BuiltinFnSETDATE()
    : Function("SETDATE", Interpreter::DataType::DATE)
{
    parameters.emplace_back("Day", Interpreter::DataType::INTEGER, false);
    parameters.emplace_back("Month", Interpreter::DataType::INTEGER, false);
    parameters.emplace_back("Year", Interpreter::DataType::INTEGER, false);
}

void Interpreter::BuiltinFnSETDATE::run(Interpreter::Context &ctx) {
    Interpreter::Variable *dayVar = ctx.getVariable("Day");
    if (dayVar == nullptr || dayVar->type != Interpreter::DataType::INTEGER) std::abort();

    Interpreter::Variable *monthVar = ctx.getVariable("Month");
    if (monthVar == nullptr || monthVar->type != Interpreter::DataType::INTEGER) std::abort();

    Interpreter::Variable *yearVar = ctx.getVariable("Year");
    if (yearVar == nullptr || yearVar->type != Interpreter::DataType::INTEGER) std::abort();

    day day(dayVar->get<Interpreter::Integer>().value);
    month month(monthVar->get<Interpreter::Integer>().value);
    year year(yearVar->get<Interpreter::Integer>().value);

    year_month_day ymd(year, month, day);
    if (!ymd.ok())
        throw Interpreter::RuntimeError(Interpreter::errToken, ctx, "Invalid Date!");

    auto ret = std::make_unique<Interpreter::Date>(ymd);

    ctx.returnValue = std::make_unique<NodeResult>(std::move(ret), Interpreter::DataType::DATE);
}

Interpreter::BuiltinFnTODAY::BuiltinFnTODAY()
    : Function("TODAY", Interpreter::DataType::DATE)
{}

void Interpreter::BuiltinFnTODAY::run(Interpreter::Context &ctx) {
    auto tp = system_clock::now();
    time_t tt = system_clock::to_time_t(tp);
    tm local_tm = *localtime(&tt);

    day day(local_tm.tm_mday);
    month month(local_tm.tm_mon + 1);
    year year(local_tm.tm_year + 1900);

    auto ret = std::make_unique<Interpreter::Date>(year_month_day(year, month, day));

    ctx.returnValue = std::make_unique<NodeResult>(std::move(ret), Interpreter::DataType::DATE);
}
