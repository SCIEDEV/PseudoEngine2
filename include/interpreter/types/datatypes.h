#pragma once
#include <string>

namespace Interpreter {
    struct DataType {
        enum class Type {
            NONE, INTEGER, REAL, BOOLEAN, CHAR, STRING, DATE, ENUM, POINTER, COMPOSITE
        };

        using enum Type;
        const std::string *name;
        Type type;

        DataType();

        DataType(Type type, const std::string *name = nullptr);

        bool operator==(Type other) const;

        bool operator==(const DataType &other) const;
    };
}
