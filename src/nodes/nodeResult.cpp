#include "pch.h"

#include "nodes/nodeResult.h"

NodeResult::NodeResult(const Interpreter::Value *data, Interpreter::DataType type)
    : data(data), type(type)
{}

NodeResult::NodeResult(std::unique_ptr<const Interpreter::Value> &&data, Interpreter::DataType type)
    : type(type)
{
    this->data = std::move(data);
}

void NodeResult::implicitCast(Interpreter::DataType target) {
    // REAL -> INTEGER
    if (target == Interpreter::DataType::REAL && type == Interpreter::DataType::INTEGER) {
        type = Interpreter::DataType::REAL;
        data = static_cast<const Interpreter::Primitive*>(data.get())->toReal();
    }

    // STRING -> CHAR
    else if (target == Interpreter::DataType::CHAR && type == Interpreter::DataType::STRING
			 && ((const Interpreter::String*) data.get())->value.length() == 1
    ) {
        type = Interpreter::DataType::CHAR;
        data = std::make_unique<Interpreter::Char>(((const Interpreter::String*) data.get())->value[0]);
    }

    // CHAR -> STRING
    else if (target == Interpreter::DataType::STRING && type == Interpreter::DataType::CHAR) {
        type = Interpreter::DataType::STRING;
        data = static_cast<const Interpreter::Primitive*>(data.get())->toString();
    }
}
