#include "pch.h"

#include "interpreter/variable.h"

using namespace Interpreter;

DataHolder::DataHolder(const std::string &name) : name(name) {}

Variable::Variable(const std::string &name, Variable *v)
    : DataHolder(name), data(v->data), ref(v), type(v->type), isConstant(v->isConstant), parent(v->parent)
{}

Variable::Variable(const std::string &name, DataType type, bool isConstant, Context *ctx, const Value *initialData)
    : DataHolder(name), data(nullptr), ref(nullptr), type(type), isConstant(isConstant), parent(ctx)
{
    if (initialData != nullptr) {
        set(const_cast<Value*>(initialData), true);
        return;
    }
    switch (type.type) {
        case DataType::INTEGER:
            data = new Interpreter::Integer();
            break;
        case DataType::REAL:
            data = new Interpreter::Real();
            break;
        case DataType::BOOLEAN:
            data = new Interpreter::Boolean();
            break;
        case DataType::CHAR:
            data = new Interpreter::Char();
            break;
        case DataType::STRING:
            data = new Interpreter::String();
            break;
        case DataType::DATE:
            data = new Interpreter::Date();
            break;
        case DataType::ENUM:
            data = new Interpreter::Enum(*type.name);
            break;
        case DataType::POINTER:
            data = new Interpreter::Pointer(*type.name);
            break;
        case DataType::COMPOSITE:
            data = new Interpreter::Composite(*type.name, *ctx);
            break;
        case DataType::NONE:
            std::abort();
    }
}

Variable::Variable(const Variable &other, Context *ctx)
    : Variable(other.name, other.type, other.isConstant, ctx, other.data)
{}

Variable::~Variable() {
    if (ref == nullptr) delete data;
}

void Variable::set(Value *_data, bool copy) {
    if (data != nullptr) delete data;
    if (!copy) {
        data = _data;
        return;
    }

    switch (type.type) {
        case DataType::INTEGER:
            data = new Interpreter::Integer(*((const Interpreter::Integer*) _data));
            break;
        case DataType::REAL:
            data = new Interpreter::Real(*((const Interpreter::Real*) _data));
            break;
        case DataType::BOOLEAN:
            data = new Interpreter::Boolean(*((const Interpreter::Boolean*) _data));
            break;
        case DataType::CHAR:
            data = new Interpreter::Char(*((const Interpreter::Char*) _data));
            break;
        case DataType::STRING:
            data = new Interpreter::String(*((const Interpreter::String*) _data));
            break;
        case DataType::DATE:
            data = new Interpreter::Date(*((const Interpreter::Date*) _data));
            break;
        case DataType::ENUM:
            data = new Interpreter::Enum(*((const Interpreter::Enum*) _data));
            break;
        case DataType::POINTER:
            data = new Interpreter::Pointer(*((const Interpreter::Pointer*) _data));
            break;
        case DataType::COMPOSITE:
            data = new Interpreter::Composite(*((const Interpreter::Composite*) _data));
            break;
        case DataType::NONE:
            std::abort();
    }
}

Variable *Variable::createReference(const std::string &refName) {
    Variable *v = ref == nullptr ? this : ref;
    return new Variable(refName, v);
}

Variable *Variable::getReference() {
    return ref;
}
