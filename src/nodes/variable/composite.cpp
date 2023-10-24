#include "pch.h"

#include "interpreter/error.h"
#include "nodes/variable/variable.h"
#include "nodes/variable/composite.h"

CompositeDefineNode::CompositeDefineNode(const Token &token, const Token &name, Interpreter::Block &initBlock)
    : Node(token), name(name), initBlock(initBlock) {}

std::unique_ptr<NodeResult> CompositeDefineNode::evaluate(Interpreter::Context &ctx) {
    if (ctx.isIdentifierType(name, false))
        throw Interpreter::RedefinitionError(token, ctx, name.value);
    
    Interpreter::CompositeTypeDefinition definition(name.value, initBlock);
    ctx.createCompositeDefinition(std::move(definition));
    return std::make_unique<NodeResult>(nullptr, Interpreter::DataType::NONE);
}
