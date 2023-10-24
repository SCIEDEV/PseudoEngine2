#include "pch.h"

#include "parser/parser.h"

Node *Parser::parseCall() {
    const Token &callToken = *currentToken;
    advance();

    if (currentToken->type != TokenType::IDENTIFIER)
        throw Interpreter::ExpectedTokenError(*currentToken, "identifier");

    const std::string &identifier = currentToken->value;
    advance();

    std::vector<Node*> args;
    if (currentToken->type == TokenType::LPAREN) {
        advance();

        if (currentToken->type == TokenType::RPAREN) {
            advance();
        } else {
            args.push_back(parseEvaluationExpression());

            while (currentToken->type == TokenType::COMMA) {
                advance();
                args.push_back(parseEvaluationExpression());
            }

            if (currentToken->type != TokenType::RPAREN)
                throw Interpreter::ExpectedTokenError(*currentToken, "')'");
            advance();
        }
    }

    return create<CallNode>(callToken, identifier, std::move(args));
}

Node *Parser::parseProcedure() {
    const Token &procedureToken = *currentToken;
    advance();

    if (currentToken->type != TokenType::IDENTIFIER)
        throw Interpreter::ExpectedTokenError(*currentToken, "identifier");
    const std::string &procedureName = currentToken->value;
    advance();

    bool byRef = false;
    std::vector<std::string> parameterNames;
    std::vector<const Token*> parameterTypes;
    std::vector<bool> parameterPassTypes;

    if (currentToken->type == TokenType::LPAREN) {
        advance();

        int typeCount = 1;
        int passTypeCount = 0;
        while (currentToken->type != TokenType::RPAREN) {
            if (parameterNames.size() > 0) {
                if (currentToken->type != TokenType::COMMA)
                    throw Interpreter::ExpectedTokenError(*currentToken, "','");
                advance();
            }

            if (currentToken->type == TokenType::BYREF
                || currentToken->type == TokenType::BYVAL
            ) {
                TokenType currentType = byRef ? TokenType::BYREF : TokenType::BYVAL;
                if (currentType != currentToken->type) {
                    parameterPassTypes.reserve(passTypeCount);
                    for (int i = 0; i < passTypeCount; i++)
                        parameterPassTypes.push_back(byRef);
                    byRef = !byRef;
                    passTypeCount = 1;
                } else {
                    passTypeCount++;
                }
                advance();
            } else {
                passTypeCount++;
            }

            if (currentToken->type != TokenType::IDENTIFIER)
                throw Interpreter::ExpectedTokenError(*currentToken, "identifier or ')'");
            const std::string &paramName = currentToken->value;
            advance();

            if (currentToken->type == TokenType::COLON) advance();
            else if (currentToken->type == TokenType::COMMA) {
                parameterNames.emplace_back(paramName);
                typeCount++;
                continue;
            }
            else throw Interpreter::ExpectedTokenError(*currentToken, "':'");

            if (currentToken->type != TokenType::DATA_TYPE && currentToken->type != TokenType::IDENTIFIER)
                throw Interpreter::ExpectedTokenError(*currentToken, "data type");
            const Token *type = currentToken;
            advance();

            parameterNames.emplace_back(paramName);
            parameterTypes.reserve(typeCount);
            for (int i = 0; i < typeCount; i++) {
                parameterTypes.emplace_back(type);
            }
            typeCount = 1;
        }
        if (typeCount != 1) throw Interpreter::ExpectedTokenError(*currentToken, "data type");
        if (passTypeCount > 0) {
            parameterPassTypes.reserve(passTypeCount);
            for (int i = 0; i < passTypeCount; i++)
                parameterPassTypes.push_back(byRef);
        }
        advance(); // ')'
    }

    Interpreter::Block *block = parseBlock();
    if (currentToken->type != TokenType::ENDPROCEDURE)
        throw Interpreter::ExpectedTokenError(*currentToken, "'ENDPROCEDURE'");
    advance();

    return create<ProcedureNode>(
        procedureToken,
        procedureName,
        std::move(parameterNames),
        std::move(parameterTypes),
        std::move(parameterPassTypes),
        *block
    );
}
