#include "pch.h"

#include "interpreter/types/types.h"

std::ostream &Interpreter::operator<<(std::ostream &os, Interpreter::DataType &type) {
    switch (type.type) {
        case Interpreter::DataType::NONE:
            os << "None";
            break;
        case Interpreter::DataType::INTEGER:
            os << "Integer";
            break;
        case Interpreter::DataType::REAL:
            os << "Real";
            break;
        case Interpreter::DataType::BOOLEAN:
            os << "Boolean";
            break;
        case Interpreter::DataType::CHAR:
            os << "Char";
            break;
        case Interpreter::DataType::STRING:
            os << "String";
            break;
        case Interpreter::DataType::DATE:
            os << "Date";
            break;
        case Interpreter::DataType::ENUM:
            os << "Enum";
            if (type.name != nullptr) os << "(" << *type.name << ")";
            break;
        case Interpreter::DataType::POINTER:
            os << "Pointer";
            if (type.name != nullptr) os << "(" << *type.name << ")";
            break;
        case Interpreter::DataType::COMPOSITE:
            os << "Composite";
            if (type.name != nullptr) os << "(" << *type.name << ")";
            break;
    }
    return os;
}
