#pragma once
#include "nodes/base.h"

class EnumDefineNode : public Node {
private:
    const Token &name;
    std::vector<std::string> values;

public:
    EnumDefineNode(const Token &token, const Token &name, std::vector<std::string> &&values);

    std::unique_ptr<NodeResult> evaluate(Interpreter::Context &ctx) override;
};
