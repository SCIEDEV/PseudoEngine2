#include "pch.h"

#include "interpreter/builtinFunctions/functions.h"
#include "interpreter/scope/context.h"

using namespace Interpreter;

Context::Context(Context *parent, const std::string &name)
    : Context(parent, name, false, Interpreter::DataType::NONE)
{}

Context::Context(Context *parent, const std::string &name, bool isFunctionCtx, Interpreter::DataType returnType)
    : parent(parent), name(name), isFunctionCtx(isFunctionCtx), isCompositeCtx(false), returnType(returnType)
{}

Context::Context(Context *parent, const std::string &name, bool isCompositeCtx)
    : parent(parent), name(name), isFunctionCtx(false), isCompositeCtx(isCompositeCtx), returnType(Interpreter::DataType::NONE)
{}

template<typename T, typename... Args>
void copyPtrVector(const std::vector<std::unique_ptr<T>> &source, std::vector<std::unique_ptr<T>> &dest, Args&&... args) {
    size_t size = source.size();
    dest.reserve(size);
    for (size_t i = 0; i < size; i++) {
        dest.emplace_back(std::make_unique<T>(*source[i], std::forward<Args>(args)...));
    }
}

Context::Context(const Context &other)
    : parent(other.parent),
    name(other.name),
    isFunctionCtx(other.isFunctionCtx),
    isCompositeCtx(other.isCompositeCtx),
    returnType(other.returnType)
{
    copyPtrVector(other.variables, variables, this);
    copyPtrVector(other.arrays, arrays);
    copyPtrVector(other.procedures, procedures);
    copyPtrVector(other.functions, functions);
}

void Context::copyVariableData(const Context &other) {
    for (size_t i = 0; i < variables.size(); i++) {
        variables[i]->set(&other.variables[i]->get<Value>(), true);
    }
}

std::unique_ptr<Context> Context::createGlobalContext() {
    auto ctx = std::make_unique<Context>(nullptr, "Program");

    ctx->functions.reserve(34);

    ctx->addFunction(std::make_unique<Interpreter::BuiltinFnLength>());
    ctx->addFunction(std::make_unique<Interpreter::BuiltinFnRight>());
    ctx->addFunction(std::make_unique<Interpreter::BuiltinFnMid>());
    ctx->addFunction(std::make_unique<Interpreter::BuiltinFnLeft>());
    ctx->addFunction(std::make_unique<Interpreter::BuiltinFnToUpper>());
    ctx->addFunction(std::make_unique<Interpreter::BuiltinFnToLower>());

    ctx->addFunction(std::make_unique<Interpreter::BuiltinFnNumToStr>());
    ctx->addFunction(std::make_unique<Interpreter::BuiltinFnStrToNum>());
    ctx->addFunction(std::make_unique<Interpreter::BuiltinFnIsNum>());
    ctx->addFunction(std::make_unique<Interpreter::BuiltinFnEOF>());

    ctx->addFunction(std::make_unique<Interpreter::BuiltinFnLCase>());
    ctx->addFunction(std::make_unique<Interpreter::BuiltinFnUCase>());
    ctx->addFunction(std::make_unique<Interpreter::BuiltinFnASC>());
    ctx->addFunction(std::make_unique<Interpreter::BuiltinFnCHR>());

    ctx->addFunction(std::make_unique<Interpreter::BuiltinFnDAY>());
    ctx->addFunction(std::make_unique<Interpreter::BuiltinFnMONTH>());
    ctx->addFunction(std::make_unique<Interpreter::BuiltinFnYEAR>());
    ctx->addFunction(std::make_unique<Interpreter::BuiltinFnDAYINDEX>());
    ctx->addFunction(std::make_unique<Interpreter::BuiltinFnSETDATE>());
    ctx->addFunction(std::make_unique<Interpreter::BuiltinFnTODAY>());

    ctx->addFunction(std::make_unique<Interpreter::BuiltinFnRand>());
    ctx->addFunction(std::make_unique<Interpreter::BuiltinFnInt>());

    ctx->addFunction(std::make_unique<Interpreter::BuiltinFnPow>());
    ctx->addFunction(std::make_unique<Interpreter::BuiltinFnExp>());
    ctx->addFunction(std::make_unique<Interpreter::BuiltinFnSin>());
    ctx->addFunction(std::make_unique<Interpreter::BuiltinFnCos>());
    ctx->addFunction(std::make_unique<Interpreter::BuiltinFnTan>());
    ctx->addFunction(std::make_unique<Interpreter::BuiltinFnASin>());
    ctx->addFunction(std::make_unique<Interpreter::BuiltinFnACos>());
    ctx->addFunction(std::make_unique<Interpreter::BuiltinFnATan>());
    ctx->addFunction(std::make_unique<Interpreter::BuiltinFnATan2>());
    ctx->addFunction(std::make_unique<Interpreter::BuiltinFnSqrt>());
    ctx->addFunction(std::make_unique<Interpreter::BuiltinFnLog>());
    ctx->addFunction(std::make_unique<Interpreter::BuiltinFnLn>());

    ctx->fileManager = std::make_unique<FileManager>();

    return ctx;
}

Context *Context::getParent() const {
    return parent;
}

Context *Context::getGlobalContext() {
    Context *ctx = this;
    while (ctx->parent != nullptr) {ctx = ctx->parent;}
    return ctx;
}

const std::string &Context::getName() const {
    return name;
}

void Context::addVariable(Variable *variable) {
    variables.emplace_back(variable);
}

Variable *Context::getVariable(const std::string &varName, bool global) {
    for (auto &var : variables) {
        if (var->name == varName) return var.get();
    }

    if (!global || parent == nullptr) return nullptr;
    return getGlobalContext()->getVariable(varName);
}

void Context::addProcedure(std::unique_ptr<Procedure> &&procedure) {
    procedures.emplace_back(std::move(procedure));
}

Procedure *Context::getProcedure(const std::string &procedureName) {
    if (parent != nullptr) return parent->getProcedure(procedureName);

    for (auto &procedure : procedures) {
        if (procedureName == procedure->name) {
            return procedure.get();
        }
    }

    return nullptr;
}

void Context::addFunction(std::unique_ptr<Function> &&function) {
    functions.emplace_back(std::move(function));
}

Function *Context::getFunction(const std::string &functionName) {
    if (parent != nullptr) return parent->getFunction(functionName);

    for (auto &function : functions) {
        if (functionName == function->name) {
            return function.get();
        }
    }

    return nullptr;
}

void Context::addArray(std::unique_ptr<Array> &&array) {
    arrays.emplace_back(std::move(array));
}

Array *Context::getArray(const std::string &arrayName, bool global) {
    for (auto &array : arrays) {
        if (arrayName == array->name) {
            return array.get();
        }
    }

    if (!global || parent == nullptr) return nullptr;
    return getGlobalContext()->getArray(arrayName);
}

Interpreter::DataType Context::getType(const Token &token, bool global) {
    if (token.type == TokenType::DATA_TYPE) {
        if (token.value == "INTEGER") return Interpreter::DataType(Interpreter::DataType::INTEGER);
        else if (token.value == "REAL") return Interpreter::DataType(Interpreter::DataType::REAL);
        else if (token.value == "BOOLEAN") return Interpreter::DataType(Interpreter::DataType::BOOLEAN);
        else if (token.value == "CHAR") return Interpreter::DataType(Interpreter::DataType::CHAR);
        else if (token.value == "STRING") return Interpreter::DataType(Interpreter::DataType::STRING);
        else if (token.value == "DATE") return Interpreter::DataType(Interpreter::DataType::DATE);
        else std::abort();
    } else if (token.type == TokenType::IDENTIFIER) {
        auto *enumDefinition = getEnumDefinition(token.value, global);
        if (enumDefinition != nullptr)
            return Interpreter::DataType(Interpreter::DataType::ENUM, &enumDefinition->name);
        
        auto *pointerDefinition = getPointerDefinition(token.value, global);
        if (pointerDefinition != nullptr)
            return Interpreter::DataType(Interpreter::DataType::POINTER, &pointerDefinition->name);
        
        auto *compositeDefinition = getCompositeDefinition(token.value, global);
        if (compositeDefinition != nullptr)
            return Interpreter::DataType(Interpreter::DataType::COMPOSITE, &compositeDefinition->name);
        
        return Interpreter::DataType(Interpreter::DataType::NONE);
    }
    std::abort();
}

bool Context::isIdentifierType(const Token &identifier, bool global) {
    Interpreter::DataType dataType = getType(identifier, global);
    if (dataType != Interpreter::DataType::NONE)
        return true;

    std::unique_ptr<Interpreter::Enum> enumEl(getEnumElement(identifier.value, global));
    if (enumEl.get() != nullptr)
        return true;

    return false;
}

Enum *Context::getEnumElement(const std::string &value, bool global) {
    for (auto &definition : enums) {
        for (size_t i = 0; i < definition->values.size(); i++) {
            if (definition->values[i] == value) {
                auto ptr = new Enum(definition->name);
                ptr->idx = i;
                return ptr;
            }
        }
    }
    if (global && parent != nullptr) return getGlobalContext()->getEnumElement(value);
    return nullptr;
}

void Context::createEnumDefinition(EnumTypeDefinition &&definition) {
    enums.emplace_back(std::make_unique<EnumTypeDefinition>(std::move(definition)));
}

void Context::createPointerDefinition(PointerTypeDefinition &&definition) {
    pointers.emplace_back(std::make_unique<PointerTypeDefinition>(std::move(definition)));
}

void Context::createCompositeDefinition(CompositeTypeDefinition &&definition) {
    composites.emplace_back(std::make_unique<CompositeTypeDefinition>(std::move(definition)));
}

const EnumTypeDefinition *Context::getEnumDefinition(const std::string &name, bool global) {
    for (const auto &e : enums) {
        if (e->name == name) return e.get();
    }
    if (global && parent != nullptr) return getGlobalContext()->getEnumDefinition(name);
    return nullptr;
}

const PointerTypeDefinition *Context::getPointerDefinition(const std::string &name, bool global) {
    for (const auto &p : pointers) {
        if (p->name == name) return p.get();
    }
    if (global && parent != nullptr) return getGlobalContext()->getPointerDefinition(name);
    return nullptr;
}

const CompositeTypeDefinition *Context::getCompositeDefinition(const std::string &name, bool global) {
    for (const auto &c : composites) {
        if (c->name == name) return c.get();
    }
    if (global && parent != nullptr) return getGlobalContext()->getCompositeDefinition(name);
    return nullptr;
}

FileManager &Context::getFileManager() {
    return *(getGlobalContext()->fileManager);
}
