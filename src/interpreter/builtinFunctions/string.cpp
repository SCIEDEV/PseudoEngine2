#include "pch.h"
#include <algorithm>

#include "interpreter/variable.h"
#include "interpreter/file.h"
#include "interpreter/error.h"
#include "interpreter/scope/context.h"
#include "interpreter/builtinFunctions/functions.h"

Interpreter::BuiltinFnLength::BuiltinFnLength()
    : Function("LENGTH", Interpreter::DataType::INTEGER)
{
    parameters.emplace_back("String", Interpreter::DataType::STRING, false);
}

void Interpreter::BuiltinFnLength::run(Interpreter::Context &ctx) {
    Interpreter::Variable *var = ctx.getVariable("String");
    if (var == nullptr || var->type != Interpreter::DataType::STRING) std::abort();

    int_t len = (int_t) var->get<Interpreter::String>().value.size();
    auto ret = std::make_unique<Interpreter::Integer>(len);

    ctx.returnValue = std::make_unique<NodeResult>(std::move(ret), Interpreter::DataType::INTEGER);
}


Interpreter::BuiltinFnRight::BuiltinFnRight()
    : Function("RIGHT", Interpreter::DataType::STRING)
{
    parameters.reserve(2);
    parameters.emplace_back("String", Interpreter::DataType::STRING, false);
    parameters.emplace_back("x", Interpreter::DataType::INTEGER, false);
}

void Interpreter::BuiltinFnRight::run(Interpreter::Context &ctx) {
    Interpreter::Variable *str = ctx.getVariable("String");
    if (str == nullptr || str->type != Interpreter::DataType::STRING) std::abort();

    Interpreter::Variable *x = ctx.getVariable("x");
    if (x == nullptr || x->type != Interpreter::DataType::INTEGER) std::abort();

    std::string &strVal = str->get<Interpreter::String>().value;
    size_t strLen = strVal.size();

    int_t xVal = x->get<Interpreter::Integer>().value;
    if (xVal < 0)
        throw Interpreter::RuntimeError(Interpreter::errToken, ctx, "Length for 'RIGHT' function cannot be negative");
    if (static_cast<size_t>(xVal) > strLen)
        throw Interpreter::RuntimeError(Interpreter::errToken, ctx, "Length for 'RIGHT' function cannot exceed string length");

    auto ret = std::make_unique<Interpreter::String>();

    size_t start = strLen - xVal;
    for (size_t i = start > 0 ? start : 0; i < strLen; i++) {
        ret->value += strVal[i];
    }

    ctx.returnValue = std::make_unique<NodeResult>(std::move(ret), Interpreter::DataType::STRING);
}


Interpreter::BuiltinFnMid::BuiltinFnMid()
    : Function("MID", Interpreter::DataType::STRING)
{
    parameters.reserve(3);
    parameters.emplace_back("String", Interpreter::DataType::STRING, false);
    parameters.emplace_back("x", Interpreter::DataType::INTEGER, false);
    parameters.emplace_back("y", Interpreter::DataType::INTEGER, false);
}

void Interpreter::BuiltinFnMid::run(Interpreter::Context &ctx) {
    Interpreter::Variable *str = ctx.getVariable("String");
    if (str == nullptr || str->type != Interpreter::DataType::STRING) std::abort();

    Interpreter::Variable *x = ctx.getVariable("x");
    if (x == nullptr || x->type != Interpreter::DataType::INTEGER) std::abort();

    Interpreter::Variable *y = ctx.getVariable("y");
    if (y == nullptr || y->type != Interpreter::DataType::INTEGER) std::abort();


    auto ret = std::make_unique<Interpreter::String>();
    std::string &strVal = str->get<Interpreter::String>().value;
    size_t strLen = strVal.size();

    int_t xVal = x->get<Interpreter::Integer>().value - 1;
    if (xVal < 0)
        throw Interpreter::RuntimeError(Interpreter::errToken, ctx, "Index for 'MID' function cannot be negative");
    if (static_cast<size_t>(xVal) >= strLen)
        throw Interpreter::RuntimeError(Interpreter::errToken, ctx, "Index for 'MID' function cannot exceed string length");

    int_t yVal = y->get<Interpreter::Integer>().value;
    if (yVal < 0)
        throw Interpreter::RuntimeError(Interpreter::errToken, ctx, "Length for 'MID' function cannot be negative");
    if (static_cast<size_t>(yVal + xVal) > strLen)
        throw Interpreter::RuntimeError(Interpreter::errToken, ctx, "Substring length in 'MID' function cannot exceed string length");

    ret->value = strVal.substr(xVal, yVal);

    ctx.returnValue = std::make_unique<NodeResult>(std::move(ret), Interpreter::DataType::STRING);
}


Interpreter::BuiltinFnLeft::BuiltinFnLeft()
    : Function("LEFT", Interpreter::DataType::STRING)
{
    parameters.reserve(2);
    parameters.emplace_back("String", Interpreter::DataType::STRING, false);
    parameters.emplace_back("x", Interpreter::DataType::INTEGER, false);
}

void Interpreter::BuiltinFnLeft::run(Interpreter::Context &ctx) {
    Interpreter::Variable *str = ctx.getVariable("String");
    if (str == nullptr || str->type != Interpreter::DataType::STRING) std::abort();

    Interpreter::Variable *x = ctx.getVariable("x");
    if (x == nullptr || x->type != Interpreter::DataType::INTEGER) std::abort();

    std::string &strVal = str->get<Interpreter::String>().value;

    int_t xVal = x->get<Interpreter::Integer>().value;
    if (xVal < 0)
        throw Interpreter::RuntimeError(Interpreter::errToken, ctx, "Length for 'LEFT' function cannot be negative");
    if (static_cast<size_t>(xVal) > strVal.size())
        throw Interpreter::RuntimeError(Interpreter::errToken, ctx, "Length for 'LEFT' function cannot exceed string length");

    auto ret = std::make_unique<Interpreter::String>();
    ret->value = strVal.substr(0, xVal);

    ctx.returnValue = std::make_unique<NodeResult>(std::move(ret), Interpreter::DataType::STRING);
}


Interpreter::BuiltinFnToUpper::BuiltinFnToUpper()
    : Function("TO_UPPER", Interpreter::DataType::STRING)
{
    parameters.emplace_back("String", Interpreter::DataType::STRING, false);
}

void Interpreter::BuiltinFnToUpper::run(Interpreter::Context &ctx) {
    Interpreter::Variable *str = ctx.getVariable("String");
    if (str == nullptr || str->type != Interpreter::DataType::STRING) std::abort();

    auto ret = std::make_unique<Interpreter::String>();

    ret->value = str->get<Interpreter::String>().value;
    std::transform(ret->value.begin(), ret->value.end(), ret->value.begin(), toupper);

    ctx.returnValue = std::make_unique<NodeResult>(std::move(ret), Interpreter::DataType::STRING);
}


Interpreter::BuiltinFnToLower::BuiltinFnToLower()
    : Function("TO_LOWER", Interpreter::DataType::STRING)
{
    parameters.emplace_back("String", Interpreter::DataType::STRING, false);
}

void Interpreter::BuiltinFnToLower::run(Interpreter::Context &ctx) {
    Interpreter::Variable *str = ctx.getVariable("String");
    if (str == nullptr || str->type != Interpreter::DataType::STRING) std::abort();

    auto ret = std::make_unique<Interpreter::String>();

    ret->value = str->get<Interpreter::String>().value;
    std::transform(ret->value.begin(), ret->value.end(), ret->value.begin(), tolower);

    ctx.returnValue = std::make_unique<NodeResult>(std::move(ret), Interpreter::DataType::STRING);
}


Interpreter::BuiltinFnNumToStr::BuiltinFnNumToStr()
    : Function("NUM_TO_STR", Interpreter::DataType::STRING)
{
    parameters.emplace_back("x", Interpreter::DataType::REAL, false);
}

void Interpreter::BuiltinFnNumToStr::run(Interpreter::Context &ctx) {
    Interpreter::Variable *x = ctx.getVariable("x");
    if (x == nullptr || x->type != Interpreter::DataType::REAL) std::abort();

    auto ret = x->get<Interpreter::Real>().toString();

    ctx.returnValue = std::make_unique<NodeResult>(std::move(ret), Interpreter::DataType::STRING);
}


Interpreter::BuiltinFnStrToNum::BuiltinFnStrToNum()
    : Function("STR_TO_NUM", Interpreter::DataType::REAL)
{
    parameters.emplace_back("String", Interpreter::DataType::STRING, false);
}

void Interpreter::BuiltinFnStrToNum::run(Interpreter::Context &ctx) {
    Interpreter::Variable *str = ctx.getVariable("String");
    if (str == nullptr || str->type != Interpreter::DataType::STRING) std::abort();

    auto ret = str->get<Interpreter::String>().toReal();

    ctx.returnValue = std::make_unique<NodeResult>(std::move(ret), Interpreter::DataType::REAL);
}


Interpreter::BuiltinFnIsNum::BuiltinFnIsNum()
    : Function("IS_NUM", Interpreter::DataType::BOOLEAN)
{
    parameters.emplace_back("String", Interpreter::DataType::STRING, false);
}

void Interpreter::BuiltinFnIsNum::run(Interpreter::Context &ctx) {
    Interpreter::Variable *str = ctx.getVariable("String");
    if (str == nullptr || str->type != Interpreter::DataType::STRING) std::abort();

    std::string &strVal = str->get<Interpreter::String>().value;
    auto ret = std::make_unique<Interpreter::Boolean>(true);
    bool decimal = false;

    for (char &c : strVal) {
        if (c == '.') {
            if (decimal) {
                ret->value = false;
                break;
            } else {
                decimal = true;
            }
        } else if (c < '0' || c > '9') {
            ret->value = false;
            break;
        }
    }

    ctx.returnValue = std::make_unique<NodeResult>(std::move(ret), Interpreter::DataType::BOOLEAN);
}

Interpreter::BuiltinFnEOF::BuiltinFnEOF()
    : Function("EOF", Interpreter::DataType::BOOLEAN)
{
    parameters.emplace_back("File", Interpreter::DataType::STRING, false);
}

void Interpreter::BuiltinFnEOF::run(Interpreter::Context &ctx) {
    Interpreter::Variable *fileName = ctx.getVariable("File");
    if (fileName == nullptr || fileName->type != Interpreter::DataType::STRING) std::abort();

    auto &filenameStr = fileName->get<Interpreter::String>();
    Interpreter::File *file = ctx.getFileManager().getFile(filenameStr);
    
    if (file == nullptr)
        throw Interpreter::FileNotOpenError(Interpreter::errToken, ctx, filenameStr.value);
    if (file->getMode() != FileMode::READ)
        throw Interpreter::RuntimeError(Interpreter::errToken, ctx, "File is not open in READ mode");
    
    Interpreter::Boolean *eof = new Interpreter::Boolean;
    *eof = file->eof();

    ctx.returnValue = std::make_unique<NodeResult>(eof, Interpreter::DataType::BOOLEAN);
}
