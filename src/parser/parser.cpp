#include "pch.h"

#include "parser/parser.h"

void Parser::advance() {
    if (++idx < tokens->size()) currentToken = (*tokens)[idx];
}

Interpreter::DataType Parser::getPSCType() {
    if (currentToken->value == "INTEGER") {
        return Interpreter::DataType::INTEGER;
    } else if (currentToken->value == "REAL") {
        return Interpreter::DataType::REAL;
    } else if (currentToken->value == "BOOLEAN") {
        return Interpreter::DataType::BOOLEAN;
    } else if (currentToken->value == "CHAR") {
        return Interpreter::DataType::CHAR;
    } else if (currentToken->value == "STRING") {
        return Interpreter::DataType::STRING;
    } else {
        std::abort();
    }
}

bool Parser::compareNextType(unsigned int n, TokenType type) {
    if (idx + n >= tokens->size()) return false;
    return (*tokens)[idx + n]->type == type;
}

Parser::Parser(const std::vector<Token*> *tokens)
{
    setTokens(tokens);
}

void Parser::setTokens(const std::vector<Token*> *_tokens) {
    tokens = _tokens;
    idx = SIZE_MAX; // overflow to 0 on advance()
    advance();
    nodes.reserve(tokens->size() / 2);
}

Interpreter::Block *Parser::parse() {
    Interpreter::Block *block = parseBlock(BlockType::MAIN);

    if (currentToken->type != TokenType::EXPRESSION_END)
        throw Interpreter::SyntaxError(*currentToken);

    return block;
}

Interpreter::Block *Parser::parseBlock(BlockType blockType) {
    Interpreter::Block *block;
    if (blockType == BlockType::MAIN) block = new Interpreter::MainBlock();
    else block = new Interpreter::Block();

    blocks.emplace_back(block);

    while (true) {
        while (currentToken->type == TokenType::LINE_END) advance();
        if (currentToken->type == TokenType::EXPRESSION_END
            || currentToken->type == TokenType::ENDIF
            || currentToken->type == TokenType::OTHERWISE
            || currentToken->type == TokenType::ENDCASE
            || currentToken->type == TokenType::ELSE
            || currentToken->type == TokenType::ENDWHILE
            || currentToken->type == TokenType::UNTIL
            || currentToken->type == TokenType::NEXT
            || currentToken->type == TokenType::ENDPROCEDURE
            || currentToken->type == TokenType::ENDFUNCTION
        ) break;

        if (blockType == BlockType::CASE && currentToken->type != TokenType::DECLARE) {
            bool endBlock = false;
            for (size_t i = 1; i + idx < tokens->size(); i++) {
                if (compareNextType(i, TokenType::COLON)) {
                    endBlock = true;
                    break;
                } else if (compareNextType(i, TokenType::LINE_END)) {
                    break;
                }
            }
            if (endBlock) break;
        }

        Node *node;
        if (currentToken->type == TokenType::PROCEDURE) {
            if (blockType != BlockType::MAIN) throw Interpreter::SyntaxError(*currentToken, "Procedures can only be defined in the global scope");
            node = parseProcedure();
        } else if (currentToken->type == TokenType::FUNCTION) {
            if (blockType != BlockType::MAIN) throw Interpreter::SyntaxError(*currentToken, "Functions can only be defined in the global scope");
            node = parseFunction();
        } else {
            node = parseExpression();
        }

        block->addNode(node);

        if (currentToken->type != TokenType::LINE_END && currentToken->type != TokenType::EXPRESSION_END) {
            throw Interpreter::SyntaxError(*currentToken);
        }
    }

    return block;
}

Node *Parser::parseExpression() {
    switch (currentToken->type) {
        case TokenType::DECLARE:
            return parseDeclareExpression();
        case TokenType::CONSTANT:
            return parseConstDeclareExpression();
        case TokenType::TYPE:
            return parseType();
        case TokenType::IF:
            return parseIfStatement();
        case TokenType::CASE:
            return parseCaseStatement();
        case TokenType::WHILE:
            return parseWhileLoop();
        case TokenType::REPEAT:
            return parseRepeatLoop();
        case TokenType::FOR:
            return parseForLoop();
        case TokenType::CALL:
            return parseCall();
        case TokenType::OUTPUT:
            return parseOutput();
        case TokenType::READ:
        case TokenType::INPUT:
            return parseInput();
        case TokenType::OPENFILE:
            return parseOpenFile();
        case TokenType::READFILE:
            return parseReadFile();
        case TokenType::WRITEFILE:
            return parseWriteFile();
        case TokenType::CLOSEFILE:
            return parseCloseFile();
        case TokenType::RETURN: {
            const Token &returnToken = *currentToken;
            advance();
            Node *evalExpr = parseEvaluationExpression();
            return create<ReturnNode>(returnToken, *evalExpr);
        }
        case TokenType::BREAK: {
            const Token &breakToken = *currentToken;
            advance();
            return create<BreakNode>(breakToken);
        }
        case TokenType::CONTINUE: {
            const Token &continueToken = *currentToken;
            advance();
            return create<ContinueNode>(continueToken);
        }
        default:
            return parseEvaluationExpression();
    }
}
