#include "lexer/tokens.h"
#include "nodes/nodeResult.h"
#include "pch.h"

#include <chrono>
#include <memory>
#include "interpreter/error.h"
#include "nodes/eval/arithmetic.h"
#include "interpreter/types/datatypes.h"
#include "interpreter/types/type_definitions.h"
#include "interpreter/types/types.h"

IntegerNode::IntegerNode(const Token &token)
    : Node(token), valueInt(std::stol(token.value))
{}

std::unique_ptr<NodeResult> IntegerNode::evaluate(Interpreter::Context&) {
    return std::make_unique<NodeResult>(new Interpreter::Integer(valueInt), Interpreter::DataType::INTEGER);
}


RealNode::RealNode(const Token &token)
    : Node(token), valueReal(std::stod(token.value))
{}

std::unique_ptr<NodeResult> RealNode::evaluate(Interpreter::Context&) {
    return std::make_unique<NodeResult>(new Interpreter::Real(valueReal), Interpreter::DataType::REAL);
}


CharNode::CharNode(const Token &token)
    : Node(token), valueChar(token.value[0])
{}

std::unique_ptr<NodeResult> CharNode::evaluate(Interpreter::Context&) {
    return std::make_unique<NodeResult>(new Interpreter::Char(valueChar), Interpreter::DataType::CHAR);
}

StringNode::StringNode(const Token &token)
    : Node(token), valueStr(token.value)
{}

std::unique_ptr<NodeResult> StringNode::evaluate(Interpreter::Context&) {
    return std::make_unique<NodeResult>(new Interpreter::String(valueStr), Interpreter::DataType::STRING);
}

inline Interpreter::Date makeDate(const std::string &dateStr) {
    std::string dayStr, monthStr, yearStr;
    int x = 0;
    for (char c : dateStr) {
        if (c == '/') {
            x++;
            continue;
        }

        if (x == 0) dayStr += c;
        else if (x == 1) monthStr += c;
        else yearStr += c;
    }

    std::chrono::day day(std::stoul(dayStr));
    std::chrono::month month(std::stoul(monthStr));
    std::chrono::year year(std::stoul(yearStr));

    return Interpreter::Date(std::chrono::year_month_day(year, month, day));
}

DateNode::DateNode(const Token &token)
    : Node(token), valueDate(makeDate(token.value))
{}

std::unique_ptr<NodeResult> DateNode::evaluate(Interpreter::Context &ctx) {
    if (!valueDate.date.ok())
        throw Interpreter::RuntimeError(token, ctx, "Invalid Date!");
    return std::make_unique<NodeResult>(new Interpreter::Date(valueDate), Interpreter::DataType::DATE);
}


std::unique_ptr<NodeResult> NegateNode::evaluate(Interpreter::Context &ctx) {
    auto nodeResult = node.evaluate(ctx);

    if (nodeResult->type != Interpreter::DataType::INTEGER && nodeResult->type != Interpreter::DataType::REAL) {
        throw Interpreter::InvalidUsageError(token, ctx, "'-' operator, operand must be of type Integer or Real");
    }

    const Interpreter::Number &num = nodeResult->get<Interpreter::Number>();
    auto res = num * Interpreter::Integer(-1);

    Interpreter::DataType type = res->real ? Interpreter::DataType::REAL : Interpreter::DataType::INTEGER;
    return std::make_unique<NodeResult>(std::move(res), type);
}

ArithmeticOperationNode::ArithmeticOperationNode(const Token &token, Node &left, Node &right)
    : BinaryNode(token, left, right) {
    switch (token.type) {
        case TokenType::PLUS:
            op = "+";
            break;
        case TokenType::MINUS:
            op = "-";
            break;
        case TokenType::STAR:
            op = "*";
            break;
        case TokenType::SLASH:
            op = "/";
            break;
        case TokenType::MOD:
            op = "MOD";
            break;
        case TokenType::DIV:
            op = "DIV";
            break;
        default:
            std::abort();
    }
}

std::unique_ptr<NodeResult> ArithmeticOperationNode::evaluate(Interpreter::Context &ctx) {
    auto leftRes = left.evaluate(ctx);
    auto rightRes = right.evaluate(ctx);

    bool enumSwap = false;
    if (leftRes->type == Interpreter::DataType::INTEGER && rightRes->type == Interpreter::DataType::ENUM) {
        leftRes.swap(rightRes);
        enumSwap = true;
    }
    if (leftRes->type == Interpreter::DataType::ENUM && rightRes->type == Interpreter::DataType::INTEGER
		&& (token.type == TokenType::PLUS || token.type == TokenType::MINUS)) {
        const Interpreter::Enum &enumVal = leftRes->get<Interpreter::Enum>();
        const Interpreter::Integer &integer = rightRes->get<Interpreter::Integer>();

        Interpreter::int_t left, right;
        if (enumSwap) {
            left = integer.value;
            right = enumVal.idx;
        } else {
            left = enumVal.idx;
            right = integer.value;
        }

        Interpreter::int_t res;
        if (token.type == TokenType::PLUS) {
            res = left + right;
        } else {
            res = left - right;
        }

        const Interpreter::EnumTypeDefinition &definition = enumVal.getDefinition(ctx);
        std::size_t enumSize = definition.values.size();
        res %= enumSize;
        if (res < 0) res += enumSize;

        std::unique_ptr<Interpreter::Enum> resEnum = std::make_unique<Interpreter::Enum>(definition.name);
        resEnum->idx = res;
        return std::make_unique<NodeResult>(std::move(resEnum), Interpreter::DataType::ENUM);
    }

    if ((leftRes->type != Interpreter::DataType::INTEGER && leftRes->type != Interpreter::DataType::REAL)
        || (rightRes->type != Interpreter::DataType::INTEGER && rightRes->type != Interpreter::DataType::REAL)
    ) {
        throw Interpreter::InvalidUsageError(token, ctx, "'" + op + "' operator, operands must be of type Integer or Real");
    }

    const Interpreter::Number &leftNum = leftRes->get<Interpreter::Number>();
    const Interpreter::Number &rightNum = rightRes->get<Interpreter::Number>();

    std::unique_ptr<Interpreter::Number> resNum;
    switch (token.type) {
        case TokenType::PLUS:
            resNum = leftNum + rightNum;
            break;
        case TokenType::MINUS:
            resNum = leftNum - rightNum;
            break;
        case TokenType::STAR:
            resNum = leftNum * rightNum;
            break;
        case TokenType::SLASH:
            if ((rightNum.real && ((Interpreter::Real&) rightNum).value == 0)
                || (!rightNum.real && ((Interpreter::Integer&) rightNum).value == 0)
            ) throw Interpreter::RuntimeError(token, ctx, "Division by 0");
            resNum = leftNum / rightNum;
            break;
        case TokenType::MOD:
            if ((rightNum.real && ((Interpreter::Real&) rightNum).value == 0)
                || (!rightNum.real && ((Interpreter::Integer&) rightNum).value == 0)
            ) throw Interpreter::RuntimeError(token, ctx, "Modulus by 0");
            resNum = leftNum % rightNum;
            break;
        case TokenType::DIV:
        if ((rightNum.real && ((Interpreter::Real&) rightNum).value == 0)
                || (!rightNum.real && ((Interpreter::Integer&) rightNum).value == 0)
            ) throw Interpreter::RuntimeError(token, ctx, "Division by 0");
            resNum = leftNum | rightNum;
            break;
        default:
            std::abort();
    }

    Interpreter::DataType type = resNum->real ? Interpreter::DataType::REAL : Interpreter::DataType::INTEGER;
    return std::make_unique<NodeResult>(std::move(resNum), type);
}
