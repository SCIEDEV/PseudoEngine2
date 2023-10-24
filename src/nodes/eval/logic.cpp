#include "pch.h"

#include "interpreter/error.h"
#include "interpreter/types/types.h"
#include "nodes/eval/logic.h"

LogicNode::LogicNode(const Token &token, Node &left, Node &right)
    : BinaryNode(token, left, right)
{
    switch (token.type) {
        case TokenType::AND:
            op = "AND";
            break;
        case TokenType::OR:
            op = "OR";
            break;
        default:
            std::abort();
    }
}

std::unique_ptr<NodeResult> LogicNode::evaluate(Interpreter::Context &ctx) {
    auto leftRes = left.evaluate(ctx);
    auto rightRes = right.evaluate(ctx);

    if (leftRes->type != Interpreter::DataType::BOOLEAN || rightRes->type != Interpreter::DataType::BOOLEAN) {
        throw Interpreter::InvalidUsageError(token, ctx, "'" + op + "' operator, operands must be of type Boolean");
    }

    const Interpreter::Boolean &leftBool = leftRes->get<Interpreter::Boolean>();
    const Interpreter::Boolean &rightBool = rightRes->get<Interpreter::Boolean>();

    Interpreter::Boolean *res = new Interpreter::Boolean();
    switch (token.type) {
        case TokenType::AND:
            *res = leftBool && rightBool;
            break;
        case TokenType::OR:
            *res = leftBool || rightBool;
            break;
        default:
            std::abort();
    }

    return std::make_unique<NodeResult>(res, Interpreter::DataType::BOOLEAN);
}


std::unique_ptr<NodeResult> NotNode::evaluate(Interpreter::Context &ctx) {
    auto nodeRes = node.evaluate(ctx);

    if (nodeRes->type != Interpreter::DataType::BOOLEAN) {
        throw Interpreter::InvalidUsageError(token, ctx, "'NOT' operator, operand must be of type Boolean");
    }

    Interpreter::Boolean *res = new Interpreter::Boolean(!nodeRes->get<Interpreter::Boolean>());
    return std::make_unique<NodeResult>(res, Interpreter::DataType::BOOLEAN);
}
