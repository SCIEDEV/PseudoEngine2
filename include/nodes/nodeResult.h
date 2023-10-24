#pragma once
#include <memory>
#include <concepts>
#include "interpreter/types/types.h"

struct NodeResult {
    std::unique_ptr<const Interpreter::Value> data;
    Interpreter::DataType type;

    NodeResult(const Interpreter::Value *data, Interpreter::DataType type);

    NodeResult(std::unique_ptr<const Interpreter::Value> &&data, Interpreter::DataType type);

    void implicitCast(Interpreter::DataType target);

    template<std::derived_from<Interpreter::Value> T>
    const T &get() const { return *((const T*) data.get()); }
};
