#include "pch.h"

#include "parser/parser.h"

Node *Parser::parseOutput() {
    const Token &outputToken = *currentToken;
    advance();

    std::vector<Node*> nodes;
    nodes.push_back(parseEvaluationExpression());

    while (currentToken->type == TokenType::COMMA) {
        advance();
        nodes.push_back(parseEvaluationExpression());
    }

    return create<OutputNode>(outputToken, std::move(nodes));
}

Node *Parser::parseInput() {
    const Token &inputToken = *currentToken;
    advance();

    if (currentToken->type != TokenType::IDENTIFIER)
        throw Interpreter::ExpectedTokenError(*currentToken, "variable");
    auto resolver = parseIdentifierExpression();

    return create<InputNode>(inputToken, std::move(resolver));
}

Node *Parser::parseOpenFile() {
    const Token &token = *currentToken;
    advance();

    Node *filename = parseStringExpression();

    if (currentToken->type != TokenType::FOR)
        throw Interpreter::ExpectedTokenError(*currentToken, "FOR");
    advance();

    Interpreter::FileMode mode;
    switch (currentToken->type) {
        case TokenType::READ:
            mode = Interpreter::FileMode::READ;
            break;
        case TokenType::WRITE:
            mode = Interpreter::FileMode::WRITE;
            break;
        case TokenType::APPEND:
            mode = Interpreter::FileMode::APPEND;
            break;
        default:
            throw Interpreter::ExpectedTokenError(*currentToken, "READ, WRITE or APPEND");
    }
    advance();

    Node *openFileNode = create<OpenFileNode>(token, *filename, mode);
    return openFileNode;
}

Node *Parser::parseReadFile() {
    const Token &token = *currentToken;
    advance();

    Node *filename = parseStringExpression();

    if (currentToken->type != TokenType::COMMA)
        throw Interpreter::ExpectedTokenError(*currentToken, "','");
    advance();

    if (currentToken->type != TokenType::IDENTIFIER)
        throw Interpreter::ExpectedTokenError(*currentToken, "variable");
    
    Node *readFileNode = create<ReadFileNode>(token, *filename, *currentToken);
    advance();

    return readFileNode;
}

Node *Parser::parseWriteFile() {
    const Token &token = *currentToken;
    advance();

    Node *filename = parseStringExpression();

    if (currentToken->type != TokenType::COMMA)
        throw Interpreter::ExpectedTokenError(*currentToken, "','");
    advance();

    Node *data = parseEvaluationExpression();
    Node *writeFileNode = create<WriteFileNode>(token, *filename, *data);
    return writeFileNode;
}

Node *Parser::parseCloseFile() {
    const Token &token = *currentToken;
    advance();

    Node *filename = parseStringExpression();
    Node *closeFileNode = create<CloseFileNode>(token, *filename);
    return closeFileNode;
}
