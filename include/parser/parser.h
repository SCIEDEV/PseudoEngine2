#pragma once
#include <vector>
#include <concepts>
#include <memory>
#include "lexer/tokens.h"
#include "nodes/node.h"
#include "nodes/variable/resolver.h"
#include "interpreter/error.h"
#include "interpreter/scope/block.h"

class Parser {
private:
    const std::vector<Token*> *tokens;
    std::vector<std::unique_ptr<Node>> nodes;
    std::vector<std::unique_ptr<Interpreter::Block>> blocks;

    const Token *currentToken;
    size_t idx;

    void advance();

    Interpreter::DataType getPSCType();

    bool compareNextType(unsigned int n, TokenType type);

    template<std::derived_from<Node> T, typename... Args>
    inline T *create(Args&&... args) {
        T *node = new T(std::forward<Args>(args)...);
        nodes.emplace_back(node);
        return node;
    }

    enum class BlockType {
        MAIN, CASE, OTHER
    };

public:
    Parser() = default;

    Parser(const std::vector<Token*> *tokens);

    Interpreter::Block *parse();

    void setTokens(const std::vector<Token*> *_tokens);

private:
    Interpreter::Block *parseBlock(BlockType blockType = BlockType::OTHER);

    Node *parseFunction();

    Node *parseCall();

    Node *parseProcedure();

    Node *parseExpression();

    Node *parseOutput();

    Node *parseInput();

    Node *parseOpenFile();

    Node *parseReadFile();

    Node *parseWriteFile();

    Node *parseCloseFile();

    Node *parseType();

    Node *parseComposite(const Token &token, const Token &identifier);

    Node *parseForLoop();

    Node *parseRepeatLoop();

    Node *parseWhileLoop();

    Node *parseIfStatement();

    Node *parseCaseStatement();

    Node *parseDeclareExpression();

    Node *parseConstDeclareExpression();

    Node *parseArrayDeclare(const Token &declareToken, std::vector<const Token*> &identifiers);

    Node *parseEvaluationExpression();

    Node *parseLogicalExpression();

    Node *parseComparisonExpression();

    Node *parseStringExpression();

    Node *parseArithmeticExpression();

    Node *parseTerm();

    Node *parseFactor();

    Node *parseAtom();

    Node *parseCast();

    Node *parseFunctionCall();

    Node *parseArrayOperation();

    Node *parseModDivFn();

    std::unique_ptr<AbstractVariableResolver> parseIdentifierExpression();

    template<std::derived_from<Node> T>
    inline Node *parseLiteral() {
        T *node = create<T>(*currentToken);
        advance();
        return node;
    }
};
