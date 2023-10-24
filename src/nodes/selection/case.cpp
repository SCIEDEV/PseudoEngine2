#include "pch.h"

#include "interpreter/error.h"
#include "nodes/selection/case.h"

CaseComponent::CaseComponent(Interpreter::Block &block)
    : block(block)
{}


EqualsCaseComponent::EqualsCaseComponent(Interpreter::Block &block, Node &node)
    : CaseComponent(block), node(node)
{}

bool EqualsCaseComponent::match(const NodeResult &value, Interpreter::Context &ctx) {
    auto res = node.evaluate(ctx);

    if (value.type == Interpreter::DataType::REAL && res->type == Interpreter::DataType::INTEGER)
        return value.get<Interpreter::Real>().value == res->get<Interpreter::Integer>().value;

    if (value.type == Interpreter::DataType::INTEGER && res->type == Interpreter::DataType::REAL)
        return value.get<Interpreter::Integer>().value == res->get<Interpreter::Real>().value;

    if (value.type != res->type) return false;

    switch (value.type.type) {
        case Interpreter::DataType::INTEGER:
            return value.get<Interpreter::Integer>().value == res->get<Interpreter::Integer>().value;
        case Interpreter::DataType::REAL:
            return value.get<Interpreter::Real>().value == res->get<Interpreter::Real>().value;
        case Interpreter::DataType::BOOLEAN:
            return value.get<Interpreter::Boolean>().value == res->get<Interpreter::Boolean>().value;
        case Interpreter::DataType::CHAR:
            return value.get<Interpreter::Char>().value == res->get<Interpreter::Char>().value;
        case Interpreter::DataType::STRING:
            return value.get<Interpreter::String>().value == res->get<Interpreter::String>().value;
        case Interpreter::DataType::DATE:
            return value.get<Interpreter::Date>().date == res->get<Interpreter::Date>().date;
        case Interpreter::DataType::ENUM:
            return value.get<Interpreter::Enum>().idx == res->get<Interpreter::Enum>().idx;
        case Interpreter::DataType::POINTER:
            return value.get<Interpreter::Pointer>().getValue() == res->get<Interpreter::Pointer>().getValue();
        case Interpreter::DataType::COMPOSITE:
            return false;
        case Interpreter::DataType::NONE: ;
    }
    std::abort();
}


RangeCaseComponent::RangeCaseComponent(Interpreter::Block &block, Node &lowerBound, Node &upperBound)
    : CaseComponent(block), lowerBound(lowerBound), upperBound(upperBound)
{}

bool RangeCaseComponent::match(const NodeResult &value, Interpreter::Context &ctx) {
    Interpreter::real_t testValue;
    if (value.type == Interpreter::DataType::INTEGER) testValue = value.get<Interpreter::Integer>().value;
    else if (value.type == Interpreter::DataType::REAL) testValue = value.get<Interpreter::Real>().value;
    else return false;

    Interpreter::real_t lowerVal;
    auto lower = lowerBound.evaluate(ctx);

    if (lower->type == Interpreter::DataType::INTEGER) lowerVal = lower->get<Interpreter::Integer>().value;
    else if (lower->type == Interpreter::DataType::REAL) lowerVal = lower->get<Interpreter::Real>().value;
    else throw Interpreter::RuntimeError(lowerBound.getToken(), ctx, "Lower bound must be of type INTEGER or REAL");

    Interpreter::real_t upperVal;
    auto upper = upperBound.evaluate(ctx);

    if (upper->type == Interpreter::DataType::INTEGER) upperVal = upper->get<Interpreter::Integer>().value;
    else if (upper->type == Interpreter::DataType::REAL) upperVal = upper->get<Interpreter::Real>().value;
    else throw Interpreter::RuntimeError(upperBound.getToken(), ctx, "Upper bound must be of type INTEGER or REAL");

    return (lowerVal <= testValue) && (testValue <= upperVal);
}


OtherwiseCaseComponent::OtherwiseCaseComponent(Interpreter::Block &block)
    : CaseComponent(block)
{}

bool OtherwiseCaseComponent::match(const NodeResult&, Interpreter::Context&) {
    return true;
}


void CaseNode::addCase(CaseComponent *caseComponent) {
    cases.emplace_back(caseComponent);
}

std::unique_ptr<NodeResult> CaseNode::evaluate(Interpreter::Context &ctx) {
    auto value = node.evaluate(ctx);

    for (auto &c : cases) {
        if (c->match(*value, ctx)) {
            c->block.run(ctx);
            break;
        }
    }

    return std::make_unique<NodeResult>(nullptr, Interpreter::DataType::NONE);
}
