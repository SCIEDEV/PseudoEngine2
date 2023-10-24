#include "pch.h"

#include "parser/parser.h"

Node *Parser::parseForLoop() {
    const Token &forToken = *currentToken;
    advance();

    if (currentToken->type != TokenType::IDENTIFIER)
        throw Interpreter::ExpectedTokenError(*currentToken, "identifier");
    const Token &iterator = *currentToken;
    advance();

    if (currentToken->type != TokenType::ASSIGNMENT)
        throw Interpreter::ExpectedTokenError(*currentToken, "'<-'");
    advance();

    Node *start = parseArithmeticExpression();

    if (currentToken->type != TokenType::TO)
        throw Interpreter::ExpectedTokenError(*currentToken, "'TO'");
    advance();

    Node *stop = parseArithmeticExpression();

    Node *step;
    if (currentToken->type == TokenType::STEP) {
        advance();
        step = parseArithmeticExpression();
    } else {
        step = nullptr;
    }

    Interpreter::Block *block = parseBlock();

    if (currentToken->type != TokenType::NEXT)
        throw Interpreter::ExpectedTokenError(*currentToken, "'NEXT'");
    advance();

    if (currentToken->type == TokenType::IDENTIFIER) {
        if (currentToken->value != iterator.value)
            throw Interpreter::ExpectedTokenError(*currentToken, "'" + iterator.value + "'");
        advance();
    }

    return create<ForLoopNode>(forToken, iterator, *start, *stop, step, block);
}

Node *Parser::parseRepeatLoop() {
    const Token &repeatToken = *currentToken;
    advance();

    Interpreter::Block *block = parseBlock();

    if (currentToken->type != TokenType::UNTIL)
        throw Interpreter::ExpectedTokenError(*currentToken, "'UNTIL'");
    advance();

    Node *condition = parseEvaluationExpression();

    return create<RepeatUntilNode>(repeatToken, *condition, *block);
}

Node *Parser::parseWhileLoop() {
    const Token &whileToken = *currentToken;
    advance();

    Node *condition = parseEvaluationExpression();

    while (currentToken->type == TokenType::LINE_END) advance();
    if (currentToken->type == TokenType::DO) advance();

    Interpreter::Block *block = parseBlock();

    if (currentToken->type != TokenType::ENDWHILE)
        throw Interpreter::ExpectedTokenError(*currentToken, "'ENDWHILE'");
    advance();

    return create<WhileLoopNode>(whileToken, *condition, *block);
}
