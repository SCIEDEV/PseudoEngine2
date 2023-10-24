#pragma once
#include <string>
#include <vector>
#include "interpreter/types/types.h"
#include "lexer/tokens.h"

namespace Interpreter {
    class Block;
    class Context;

    struct Parameter {
        const std::string name;
        const Interpreter::DataType type;
        const bool byRef;

        Parameter(const std::string &name, Interpreter::DataType type, bool byRef);
    };

    struct Procedure {
        const std::string name;
        std::vector<Parameter> parameters;
        Interpreter::Block *const block;

        Procedure(
				const std::string &name,
				std::vector<Parameter> &&parameters,
				Interpreter::Block *block
        );

        Procedure(const std::string &name);

        virtual ~Procedure() = default;

        std::vector<DataType> getTypes() const;

        virtual void run(Interpreter::Context &ctx);
    };

    struct Function : public Procedure {
        const Interpreter::DataType returnType;
        const Token *defToken; // For throwing 'missing return' errors

        Function(
				const std::string &name,
				std::vector<Parameter> &&parameters,
				Interpreter::Block *block,
				Interpreter::DataType returnType,
				const Token *defToken
        );

        // Builtin functions
        Function(const std::string &name, Interpreter::DataType returnType);
    };
};
