#include "pch.h"

#include "interpreter/types/types.h"
#include "interpreter/types/type_definitions.h"

using namespace Interpreter;

AbstractTypeDefinition::AbstractTypeDefinition(const std::string &name)
    : name(name) {}

EnumTypeDefinition::EnumTypeDefinition(const std::string &name, std::vector<std::string> &&values)
    : AbstractTypeDefinition(name), values(std::move(values)) {}

PointerTypeDefinition::PointerTypeDefinition(const std::string &name, DataType type)
    : AbstractTypeDefinition(name), type(type) {}

CompositeTypeDefinition::CompositeTypeDefinition(const std::string &name, Interpreter::Block &initBlock)
    : AbstractTypeDefinition(name), initBlock(initBlock) {}
