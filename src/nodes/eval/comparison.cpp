#include "pch.h"

#include "interpreter/error.h"
#include "interpreter/types/types.h"
#include "nodes/eval/comparison.h"

BooleanNode::BooleanNode(const Token &token)
    : Node(token), value(token.type == TokenType::TRUE)
{
    if (!this->value && token.type != TokenType::FALSE) std::abort();
}

std::unique_ptr<NodeResult> BooleanNode::evaluate(Interpreter::Context&) {
    return std::make_unique<NodeResult>(new Interpreter::Boolean(value), Interpreter::DataType::BOOLEAN);
}


ComparisonNode::ComparisonNode(const Token &token, Node &left, Node &right)
    : BinaryNode(token, left, right)
{
    switch (token.type) {
        case TokenType::EQUALS:
            op = "=";
            break;
        case TokenType::NOT_EQUALS:
            op = "<>";
            break;
        case TokenType::GREATER:
            op = ">";
            break;
        case TokenType::LESSER:
            op = "<";
            break;
        case TokenType::GREATER_EQUAL:
            op = ">=";
            break;
        case TokenType::LESSER_EQUAL:
            op = "<=";
            break;
        default:
            std::abort();
    }
}

std::unique_ptr<NodeResult> ComparisonNode::evaluate(Interpreter::Context &ctx) {
    auto leftRes = left.evaluate(ctx);
    auto rightRes = right.evaluate(ctx);

    if (leftRes->type == Interpreter::DataType::CHAR && rightRes->type == Interpreter::DataType::CHAR) {
        leftRes->type = rightRes->type = Interpreter::DataType::INTEGER;
        leftRes->data = leftRes->get<Interpreter::Char>().toInteger();
        rightRes->data = rightRes->get<Interpreter::Char>().toInteger();
    } else if (leftRes->type == Interpreter::DataType::DATE && rightRes->type == Interpreter::DataType::DATE) {
        leftRes->type = rightRes->type = Interpreter::DataType::INTEGER;
        leftRes->data = leftRes->get<Interpreter::Date>().toInteger();
        rightRes->data = rightRes->get<Interpreter::Date>().toInteger();
    }

    if ((leftRes->type != Interpreter::DataType::INTEGER && leftRes->type != Interpreter::DataType::REAL)
        || (rightRes->type != Interpreter::DataType::INTEGER && rightRes->type != Interpreter::DataType::REAL)
    ) {
        bool eq = token.type == TokenType::EQUALS;
        if (!eq && token.type != TokenType::NOT_EQUALS)
            throw Interpreter::InvalidUsageError(token, ctx, "'" + op + "' operator, operands must be of numeric data type");

        if (leftRes->type != rightRes->type) {
            return std::make_unique<NodeResult>(new Interpreter::Boolean(!eq), Interpreter::DataType::BOOLEAN);
        } else if (leftRes->type == Interpreter::DataType::BOOLEAN) {
            bool comparisonEq = leftRes->get<Interpreter::Boolean>() == rightRes->get<Interpreter::Boolean>();
            bool res = (!eq && !comparisonEq) || (eq && comparisonEq);
            return std::make_unique<NodeResult>(new Interpreter::Boolean(res), Interpreter::DataType::BOOLEAN);
        } else if (leftRes->type == Interpreter::DataType::STRING) {
            bool comparisonEq = leftRes->get<Interpreter::String>().value == rightRes->get<Interpreter::String>().value;
            bool res = (!eq && !comparisonEq) || (eq && comparisonEq);
            return std::make_unique<NodeResult>(new Interpreter::Boolean(res), Interpreter::DataType::BOOLEAN);
        } else if (leftRes->type == Interpreter::DataType::ENUM) {
            bool comparisonEq = leftRes->get<Interpreter::Enum>().idx == rightRes->get<Interpreter::Enum>().idx;
            bool res = (!eq && !comparisonEq) || (eq && comparisonEq);
            return std::make_unique<NodeResult>(new Interpreter::Boolean(res), Interpreter::DataType::BOOLEAN);
        } else {
            throw Interpreter::InvalidUsageError(token, ctx, "'" + op + "' operator, operands must be of comparable type");
        }
    }

    const Interpreter::Number &leftNum = leftRes->get<Interpreter::Number>();
    const Interpreter::Number &rightNum = rightRes->get<Interpreter::Number>();

    std::unique_ptr<Interpreter::Boolean> res;
    switch (token.type) {
        case TokenType::EQUALS:
            res = leftNum == rightNum;
            break;
        case TokenType::NOT_EQUALS:
            res = leftNum != rightNum;
            break;
        case TokenType::GREATER:
            res = leftNum > rightNum;
            break;
        case TokenType::LESSER:
            res = leftNum < rightNum;
            break;
        case TokenType::GREATER_EQUAL:
            res = leftNum >= rightNum;
            break;
        case TokenType::LESSER_EQUAL:
            res = leftNum <= rightNum;
            break;
        default:
            std::abort();
    }

    return std::make_unique<NodeResult>(std::move(res), Interpreter::DataType::BOOLEAN);
}
