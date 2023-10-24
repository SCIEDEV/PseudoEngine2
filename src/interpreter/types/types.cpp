#include "pch.h"

#include "interpreter/types/datatypes.h"

using namespace Interpreter;

DataType::DataType() : type(Interpreter::DataType::NONE) {}

DataType::DataType(Type type, const std::string *name)
    : name(name), type(type) {}

bool DataType::operator==(Type other) const {
    return type == other;
}

bool DataType::operator==(const DataType &other) const {
    if (name == nullptr || other.name == nullptr) return operator==(other.type);
    return type == other.type && *name == *other.name;
}
