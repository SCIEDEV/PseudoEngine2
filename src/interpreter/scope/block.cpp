#include "pch.h"
#include <iostream>

#include "nodes/functions/function.h"
#include "nodes/loop/control.h"
#include "interpreter/error.h"
#include "interpreter/scope/block.h"

using namespace Interpreter;

extern bool REPLMode;

void Block::addNode(Node *node) {
    nodes.push_back(node);
}

void Block::runNodeREPL(Node *node, Interpreter::Context &ctx) {
    auto result = node->evaluate(ctx);

    switch (result->type.type) {
        case Interpreter::DataType::INTEGER:
            std::cout << result->get<Interpreter::Integer>();
            break;
        case Interpreter::DataType::REAL: {
            Interpreter::real_t value = result->get<Interpreter::Real>().value;
            std::cout << value;

            double integerValue;
            if (std::modf(value, &integerValue) == 0.0) std::cout << ".0";
            break;
        } case Interpreter::DataType::BOOLEAN:
            std::cout << (result->get<Interpreter::Boolean>() ? "TRUE" : "FALSE");
            break;
        case Interpreter::DataType::CHAR:
            std::cout << "'" << result->get<Interpreter::Char>() << "'";
            break;
        case Interpreter::DataType::STRING:
            std::cout << "\"" << result->get<Interpreter::String>().value << "\"";
            break;
        case Interpreter::DataType::DATE: {
            auto str = result->get<Interpreter::Date>().toString();
            std::cout << str->value;
            break;
        } case Interpreter::DataType::ENUM: {
            auto &resEnum = result->get<Interpreter::Enum>();
            std::cout << resEnum.definitionName << ": " << resEnum.getString(ctx);
            break;
        } case Interpreter::DataType::POINTER: {
            auto &resPtr = result->get<Interpreter::Pointer>();

            const Interpreter::Context *ptrCtx = resPtr.getCtx();
            Interpreter::Context *tempCtx = &ctx;
            bool valid = true;
            while (valid && tempCtx != ptrCtx) {
                tempCtx = tempCtx->getParent();
                if (tempCtx == nullptr) valid = false;
            }

            std::cout << resPtr.definitionName << ": ";
            if (valid) {
                Interpreter::Variable *ptrValue = resPtr.getValue();
                const std::string &valueStr = ptrValue == nullptr ? "null" : ptrValue->name;
                std::cout << valueStr;
            } else {
                std::cout << "{DELETED}";
            }
            break;
        } case Interpreter::DataType::COMPOSITE:
            std::cout << result->get<Interpreter::Composite>().definitionName << " object";
            break;
        case Interpreter::DataType::NONE:
            return;
    }
    std::cout << std::endl;
}

void Block::_run(Interpreter::Context &ctx) {
    for (Node *node : nodes) {
        node->evaluate(ctx);
    }
}

void Block::_runREPL(Interpreter::Context &ctx) {
    for (Node *node : nodes) {
        runNodeREPL(node, ctx);
    }
}

void Block::run(Interpreter::Context &ctx) {
    if (REPLMode) _runREPL(ctx);
    else _run(ctx);
}

void MainBlock::run(Interpreter::Context &ctx) {
    try {
        Block::run(ctx);
    } catch (BreakErrSignal &e) {
        throw Interpreter::InvalidUsageError(e.token, ctx, "'BREAK' statement");
    } catch (ContinueErrSignal &e) {
        throw Interpreter::InvalidUsageError(e.token, ctx, "'CONTINUE' statement");
    }
}
