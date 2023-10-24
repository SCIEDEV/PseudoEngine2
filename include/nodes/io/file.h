#pragma once
#include "nodes/base.h"
#include "interpreter/file.h"

class OpenFileNode : public Node {
private:
    const Interpreter::FileMode mode;
    Node &filename;

public:
    OpenFileNode(const Token &token, Node &filename, Interpreter::FileMode mode);

    std::unique_ptr<NodeResult> evaluate(Interpreter::Context &ctx) override;
};

class ReadFileNode : public Node {
private:
    Node &filename;
    const Token &identifier;

public:
    ReadFileNode(const Token &token, Node &filename, const Token &identifier);

    std::unique_ptr<NodeResult> evaluate(Interpreter::Context &ctx) override;
};

class WriteFileNode : public UnaryNode {
private:
    Node &filename;

public:
    WriteFileNode(const Token &token, Node &filename, Node &node);

    std::unique_ptr<NodeResult> evaluate(Interpreter::Context &ctx) override;
};

class CloseFileNode : public Node {
private:
    Node &filename;

public:
    CloseFileNode(const Token &token, Node &filename);

    std::unique_ptr<NodeResult> evaluate(Interpreter::Context &ctx) override;
};
