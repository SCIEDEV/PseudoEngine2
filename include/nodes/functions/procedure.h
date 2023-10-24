#pragma once
#include <string>
#include <vector>
#include "nodes/base.h"
#include "interpreter/scope/block.h"

class ProcedureNode : public Node {
private:
    const std::string procedureName;
    const std::vector<std::string> parameterNames;
    const std::vector<const Token*> parameterTypes;
    const std::vector<bool> parameterPassTypes;
    Interpreter::Block &block;

public:
    ProcedureNode(
			const Token &token,
			const std::string &procedureName,
			std::vector<std::string> &&parameterNames,
			std::vector<const Token*> &&parameterTypes,
			std::vector<bool> &&parameterPassTypes,
			Interpreter::Block &block
    );

    // Adds procedure to ctx
    std::unique_ptr<NodeResult> evaluate(Interpreter::Context &ctx) override;
};

class CallNode : public Node {
private:
    const std::string procedureName;
    const std::vector<Node*> args;

public:
    CallNode(const Token &token, const std::string &procedureName, std::vector<Node*> &&args);

    std::unique_ptr<NodeResult> evaluate(Interpreter::Context &ctx) override;
};
