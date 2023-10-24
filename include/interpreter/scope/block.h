#pragma once
#include <vector>
#include "interpreter/scope/context.h"
#include "nodes/base.h"

namespace Interpreter {
    class Block {
    private:
        std::vector<Node*> nodes;

        void runNodeREPL(Node *node, Interpreter::Context &ctx);

        void _run(Interpreter::Context &ctx);

        void _runREPL(Interpreter::Context &ctx);

    public:
        virtual ~Block() = default;

        void addNode(Node *node);

        virtual void run(Interpreter::Context &ctx);
    };

    class MainBlock final : public Block {
    public:
        void run(Interpreter::Context &ctx) override;
    };
}
