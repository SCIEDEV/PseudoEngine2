#include "pch.h"

#include "parser/parser.h"

Node *Parser::parseIfStatement() {
    const Token &ifToken = *currentToken;
    advance();

    Node *condition = parseEvaluationExpression();

    while (currentToken->type == TokenType::LINE_END) advance();
    if (currentToken->type != TokenType::THEN)
        throw Interpreter::ExpectedTokenError(*currentToken, "'THEN'");
    advance();

    Interpreter::Block *block = parseBlock();

    std::vector<IfConditionComponent> components;
    components.emplace_back(condition, *block);

    while (currentToken->type == TokenType::ELSE) {
        advance();
        if (currentToken->type == TokenType::IF) {
            advance();
            condition = parseEvaluationExpression();

            while (currentToken->type == TokenType::LINE_END) advance();
            if (currentToken->type != TokenType::THEN)
                throw Interpreter::ExpectedTokenError(*currentToken, "'THEN'");
            advance();

            block = parseBlock();
            components.emplace_back(condition, *block);
        } else {
            block = parseBlock();
            components.emplace_back(nullptr, *block);
            break;
        }
    }

    if (currentToken->type != TokenType::ENDIF)
        throw Interpreter::ExpectedTokenError(*currentToken, "'ENDIF'");
    advance();

    return create<IfStatementNode>(ifToken, std::move(components));
}

Node *Parser::parseCaseStatement() {
    const Token &caseToken = *currentToken;
    advance();

    if (currentToken->type != TokenType::OF)
        throw Interpreter::ExpectedTokenError(*currentToken, "'OF'");
    advance();

    if (currentToken->type != TokenType::IDENTIFIER)
        throw Interpreter::ExpectedTokenError(*currentToken, "IDENTIFIER");
    AccessNode *variable = create<AccessNode>(*currentToken, std::make_unique<SimpleVariableSource>(*currentToken));
    advance();

    while (currentToken->type == TokenType::LINE_END) advance();

    CaseNode *caseNode = create<CaseNode>(caseToken, *variable);
    while (currentToken->type != TokenType::ENDCASE) {
        if (currentToken->type == TokenType::OTHERWISE) {
            advance();

            if (currentToken->type != TokenType::COLON)
                throw Interpreter::ExpectedTokenError(*currentToken, "':'");
            advance();

            Interpreter::Block *block = parseBlock(BlockType::CASE);
            caseNode->addCase(new OtherwiseCaseComponent(*block));

            if (currentToken->type != TokenType::ENDCASE)
                throw Interpreter::ExpectedTokenError(*currentToken, "'ENDCASE'");
            break;
        }

        Node *expr = parseEvaluationExpression();
        Node *otherExpr = nullptr;

        if (currentToken->type == TokenType::TO) {
            advance();
            otherExpr = parseEvaluationExpression();
        }

        if (currentToken->type != TokenType::COLON)
            throw Interpreter::ExpectedTokenError(*currentToken, "':'");
        advance();

        Interpreter::Block *block = parseBlock(BlockType::CASE);

        CaseComponent *component;
        if (otherExpr == nullptr) {
            component = new EqualsCaseComponent(*block, *expr);
        } else {
            component = new RangeCaseComponent(*block, *expr, *otherExpr);
        }
        caseNode->addCase(component);
    }

    advance(); // ENDCASE
    return caseNode;
}
