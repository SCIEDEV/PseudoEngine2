#include "pch.h"

#include "parser/parser.h"

Node *Parser::parseType() {
    const Token &token = *currentToken;
    advance();

    while (currentToken->type == TokenType::LINE_END) advance();

    const Token &identifier = *currentToken;
    if (identifier.type != TokenType::IDENTIFIER)
        throw Interpreter::ExpectedTokenError(*currentToken, "identifier");
    advance();

    if (currentToken->type != TokenType::EQUALS) {
        if (currentToken->type != TokenType::LINE_END)
            throw Interpreter::SyntaxError(*currentToken, "Expected definition for type");
        advance();
        return parseComposite(token, identifier);
    }
    advance();

    if (currentToken->type == TokenType::CARET) {
        advance();

        const Token &pointerType = *currentToken;
        if (pointerType.type != TokenType::DATA_TYPE && pointerType.type != TokenType::IDENTIFIER)
            throw Interpreter::ExpectedTokenError(*currentToken, "data type");
        advance();

        return create<PointerDefineNode>(token, identifier, pointerType);
    } else if (currentToken->type == TokenType::LPAREN) {
        advance();
        
        std::vector<std::string> values;
        while (true) {
            if (currentToken->type != TokenType::IDENTIFIER)
                throw Interpreter::ExpectedTokenError(*currentToken, "identifier");
            values.emplace_back(currentToken->value);
            advance();

            if (currentToken->type == TokenType::COMMA)
                advance();
            else if (currentToken->type == TokenType::RPAREN)
                break;
            else
                throw Interpreter::ExpectedTokenError(*currentToken, ")");
        }
        advance(); // ')'

        return create<EnumDefineNode>(token, identifier, std::move(values));
    }

    throw Interpreter::SyntaxError(*currentToken, "Expected definition for type");
}

Node *Parser::parseComposite(const Token &token, const Token &identifier) {
    Interpreter::Block *block = new Interpreter::Block();
    blocks.emplace_back(block);

    while (currentToken->type == TokenType::DECLARE) {
        Node *declareNode = parseDeclareExpression();
        block->addNode(declareNode);

        if (currentToken->type != TokenType::LINE_END)
            throw Interpreter::ExpectedTokenError(*currentToken, "newline");
        advance();
    }

    if (currentToken->type != TokenType::ENDTYPE)
        throw Interpreter::ExpectedTokenError(*currentToken, "'ENDTYPE'");
    advance();

    return create<CompositeDefineNode>(token, identifier, *block);
}
