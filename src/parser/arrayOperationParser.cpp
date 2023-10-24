#include "pch.h"

#include "parser/parser.h"

Node *Parser::parseArrayDeclare(const Token &declareToken, std::vector<const Token*> &identifiers) {
    if (currentToken->type != TokenType::ARRAY) std::abort();
    advance();

    if (currentToken->type != TokenType::LSQRBRACKET)
        throw Interpreter::ExpectedTokenError(*currentToken, "'['");
    advance();

    std::vector<Node*> bounds;

    while (true) {
        Node *lowerBound = parseArithmeticExpression();

        if (currentToken->type != TokenType::COLON)
            throw Interpreter::ExpectedTokenError(*currentToken, "':'");
        advance();

        Node *upperBound = parseArithmeticExpression();

        bounds.push_back(lowerBound);
        bounds.push_back(upperBound);

        if (currentToken->type == TokenType::COMMA) advance();
        else break;
    }

    if (currentToken->type != TokenType::RSQRBRACKET)
        throw Interpreter::ExpectedTokenError(*currentToken, "']'");
    advance();

    if (currentToken->type != TokenType::OF)
        throw Interpreter::ExpectedTokenError(*currentToken, "'OF'");
    advance();

    if (currentToken->type != TokenType::DATA_TYPE && currentToken->type != TokenType::IDENTIFIER)
        throw Interpreter::ExpectedTokenError(*currentToken, "data type");

    const Token &type = *currentToken;
    advance();

    return create<ArrayDeclareNode>(declareToken, std::move(identifiers), type, std::move(bounds));
}
