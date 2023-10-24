#include "pch.h"

#include "nodes/loop/control.h"

BreakErrSignal::BreakErrSignal(const Token &token) : token(token) {}

ContinueErrSignal::ContinueErrSignal(const Token &token) : token(token) {}

std::unique_ptr<NodeResult> BreakNode::evaluate(Interpreter::Context&) {
    throw BreakErrSignal(token);
}

std::unique_ptr<NodeResult> ContinueNode::evaluate(Interpreter::Context&) {
    throw ContinueErrSignal(token);
}
