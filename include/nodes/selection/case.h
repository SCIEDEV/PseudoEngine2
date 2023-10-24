#pragma once
#include <vector>
#include "nodes/base.h"
#include "interpreter/scope/block.h"

class CaseComponent {
public:
    Interpreter::Block &block;

    CaseComponent(Interpreter::Block &block);

    virtual ~CaseComponent() = default;

    virtual bool match(const NodeResult &value, Interpreter::Context &ctx) = 0;
};

class EqualsCaseComponent : public CaseComponent {
private:
    Node &node;

public:
    EqualsCaseComponent(Interpreter::Block &block, Node &node);

    bool match(const NodeResult &value, Interpreter::Context &ctx) override;
};

class RangeCaseComponent : public CaseComponent {
private:
    Node &lowerBound, &upperBound;

public:
    RangeCaseComponent(Interpreter::Block &block, Node &lowerBound, Node &upperBound);

    bool match(const NodeResult &value, Interpreter::Context &ctx) override;
};

class OtherwiseCaseComponent : public CaseComponent {
public:
    OtherwiseCaseComponent(Interpreter::Block &block);

    bool match(const NodeResult &value, Interpreter::Context &ctx) override;
};

class CaseNode : public UnaryNode {
private:
    std::vector<std::unique_ptr<CaseComponent>> cases;

public:
    using UnaryNode::UnaryNode;

    void addCase(CaseComponent *caseComponent);

    std::unique_ptr<NodeResult> evaluate(Interpreter::Context &ctx) override;
};
