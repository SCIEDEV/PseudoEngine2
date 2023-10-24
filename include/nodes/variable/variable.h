#pragma once
#include "nodes/base.h"
#include "nodes/variable/resolver.h"
#include "interpreter/error.h"

class DeclareNode : public Node {
private:
    const std::vector<const Token*> identifiers;
    const Token &type;

public:
    // token: DECLARE
    DeclareNode(const Token &token, std::vector<const Token*> &&identifiers, const Token &type);

    std::unique_ptr<NodeResult> evaluate(Interpreter::Context &ctx) override;
};

class ConstDeclareNode : public UnaryNode {
private:
    const Token &identifier;

public:
    // token: CONST
    ConstDeclareNode(const Token &token, Node &node, const Token &identifier);

    std::unique_ptr<NodeResult> evaluate(Interpreter::Context &ctx) override;
};

class AssignNode : public UnaryNode {
private:
    const std::unique_ptr<AbstractVariableResolver> resolver;

    void assignArray(Interpreter::Context &ctx, const Interpreter::ArrayDirectAccessError &e);

public:
    // token: ASSIGNMENT
    AssignNode(const Token &token, Node &node, std::unique_ptr<AbstractVariableResolver> &&resolver);

    std::unique_ptr<NodeResult> evaluate(Interpreter::Context &ctx) override;
};

class AccessNode : public Node {
private:
    const std::unique_ptr<AbstractVariableResolver> resolver;
    friend AssignNode;

public:
    // token: IDENTIFIER
    AccessNode(const Token &token, std::unique_ptr<AbstractVariableResolver> &&resolver);

    std::unique_ptr<NodeResult> evaluate(Interpreter::Context &ctx) override;

    // For BYREF
    const AbstractVariableResolver &getResolver() const;
};
