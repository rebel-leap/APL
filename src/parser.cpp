#include "apl/parser.h"
#include <stdexcept>

namespace apl {
    Parser::Parser(const Vector<Token>& tokens) : tokens(tokens) {}

    Token& Parser::peek(size_t offset) {
        if (pos + offset >= tokens.size()) return tokens.back();
        return tokens[pos + offset];
    }

    Token& Parser::advance() {
        if (!isAtEnd()) pos++;
        return tokens[pos - 1];
    }

    bool Parser::match(TokenType type) {
        if (check(type)) { advance(); return true; }
        return false;
    }

    bool Parser::check(TokenType type) {
        if (isAtEnd()) return false;
        return peek().type == type;
    }

    Token& Parser::consume(TokenType type, const String& message) {
        if (check(type)) return advance();
        throw std::runtime_error(message);
    }

    bool Parser::isAtEnd() {
        return peek().type == TokenType::EOF_TOKEN;
    }

    Vector<StmtPtr> Parser::parse() {
        Vector<StmtPtr> statements;
        while (!isAtEnd()) {
            if (match(TokenType::NEWLINE)) continue;
            statements.push_back(declaration());
        }
        return statements;
    }

    StmtPtr Parser::declaration() {
        if (match(TokenType::VAR)) return varDeclaration();
        if (match(TokenType::FUNC)) return funcDeclaration();
        if (match(TokenType::CLASS)) return classDeclaration();
        if (match(TokenType::IMPORT)) return importStatement();
        return statement();
    }

    StmtPtr Parser::statement() {
        if (match(TokenType::IF)) return ifStatement();
        if (match(TokenType::WHILE)) return whileStatement();
        if (match(TokenType::FOR)) return forStatement();
        if (match(TokenType::RETURN)) return returnStatement();
        if (match(TokenType::LBRACE)) return blockStatement();
        return expressionStatement();
    }

    StmtPtr Parser::varDeclaration() {
        auto stmt = std::make_shared<VarDeclStmt>();
        stmt->name = consume(TokenType::IDENTIFIER, "Expected variable name").lexeme;
        if (match(TokenType::ASSIGN)) {
            stmt->initializer = expression();
        }
        return stmt;
    }

    StmtPtr Parser::funcDeclaration() {
        auto stmt = std::make_shared<FuncDeclStmt>();
        stmt->name = consume(TokenType::IDENTIFIER, "Expected function name").lexeme;
        consume(TokenType::LPAREN, "Expected '(' after function name");
        if (!check(TokenType::RPAREN)) {
            do {
                stmt->params.push_back(consume(TokenType::IDENTIFIER, "Expected parameter name").lexeme);
            } while (match(TokenType::COMMA));
        }
        consume(TokenType::RPAREN, "Expected ')' after parameters");
        stmt->body = blockStatement();
        return stmt;
    }

    StmtPtr Parser::classDeclaration() {
        auto stmt = std::make_shared<ClassDeclStmt>();
        stmt->name = consume(TokenType::IDENTIFIER, "Expected class name").lexeme;
        consume(TokenType::LBRACE, "Expected '{' before class body");
        while (!check(TokenType::RBRACE) && !isAtEnd()) {
            if (match(TokenType::NEWLINE)) continue;
            auto method = std::make_shared<FuncDeclStmt>();
            method->name = consume(TokenType::IDENTIFIER, "Expected method name").lexeme;
            consume(TokenType::LPAREN, "Expected '(' after method name");
            if (!check(TokenType::RPAREN)) {
                do {
                    method->params.push_back(consume(TokenType::IDENTIFIER, "Expected parameter name").lexeme);
                } while (match(TokenType::COMMA));
            }
            consume(TokenType::RPAREN, "Expected ')' after parameters");
            method->body = blockStatement();
            stmt->methods.push_back({method->name, *method});
        }
        consume(TokenType::RBRACE, "Expected '}' after class body");
        return stmt;
    }

    StmtPtr Parser::ifStatement() {
        auto stmt = std::make_shared<IfStmt>();
        stmt->condition = expression();
        stmt->thenBranch = statement();
        if (match(TokenType::ELSE)) {
            stmt->elseBranch = statement();
        }
        return stmt;
    }

    StmtPtr Parser::whileStatement() {
        auto stmt = std::make_shared<WhileStmt>();
        stmt->condition = expression();
        stmt->body = statement();
        return stmt;
    }

    StmtPtr Parser::forStatement() {
        auto stmt = std::make_shared<ForStmt>();
        stmt->varName = consume(TokenType::IDENTIFIER, "Expected variable name").lexeme;
        consume(TokenType::IN, "Expected 'in' after for variable");
        stmt->iterable = expression();
        stmt->body = statement();
        return stmt;
    }

    StmtPtr Parser::returnStatement() {
        auto stmt = std::make_shared<ReturnStmt>();
        if (!check(TokenType::NEWLINE) && !check(TokenType::RBRACE) && !isAtEnd()) {
            stmt->value = expression();
        }
        return stmt;
    }

    StmtPtr Parser::importStatement() {
        auto stmt = std::make_shared<ImportStmt>();
        stmt->path = consume(TokenType::STRING, "Expected import path").lexeme;
        return stmt;
    }

    StmtPtr Parser::expressionStatement() {
        auto stmt = std::make_shared<ExpressionStmt>();
        stmt->expression = expression();
        return stmt;
    }

    StmtPtr Parser::blockStatement() {
        auto stmt = std::make_shared<BlockStmt>();
        while (!check(TokenType::RBRACE) && !isAtEnd()) {
            if (match(TokenType::NEWLINE)) continue;
            stmt->statements.push_back(declaration());
        }
        consume(TokenType::RBRACE, "Expected '}' after block");
        return stmt;
    }

    ExprPtr Parser::expression() { return assignment(); }

    ExprPtr Parser::assignment() {
        ExprPtr expr = orExpr();
        if (match(TokenType::ASSIGN)) {
            auto assign = std::make_shared<AssignExpr>();
            assign->target = expr;
            assign->value = assignment();
            return assign;
        }
        return expr;
    }

    ExprPtr Parser::orExpr() {
        ExprPtr expr = andExpr();
        while (match(TokenType::OR)) {
            auto binary = std::make_shared<BinaryExpr>();
            binary->left = expr;
            binary->op = TokenType::OR;
            binary->right = andExpr();
            expr = binary;
        }
        return expr;
    }

    ExprPtr Parser::andExpr() {
        ExprPtr expr = equality();
        while (match(TokenType::AND)) {
            auto binary = std::make_shared<BinaryExpr>();
            binary->left = expr;
            binary->op = TokenType::AND;
            binary->right = equality();
            expr = binary;
        }
        return expr;
    }

    ExprPtr Parser::equality() {
        ExprPtr expr = comparison();
        while (match(TokenType::EQ) || match(TokenType::NE)) {
            auto binary = std::make_shared<BinaryExpr>();
            binary->left = expr;
            binary->op = tokens[pos - 1].type;
            binary->right = comparison();
            expr = binary;
        }
        return expr;
    }

    ExprPtr Parser::comparison() {
        ExprPtr expr = term();
        while (match(TokenType::LT) || match(TokenType::GT) || match(TokenType::LE) || match(TokenType::GE)) {
            auto binary = std::make_shared<BinaryExpr>();
            binary->left = expr;
            binary->op = tokens[pos - 1].type;
            binary->right = term();
            expr = binary;
        }
        return expr;
    }

    ExprPtr Parser::term() {
        ExprPtr expr = factor();
        while (match(TokenType::PLUS) || match(TokenType::MINUS)) {
            auto binary = std::make_shared<BinaryExpr>();
            binary->left = expr;
            binary->op = tokens[pos - 1].type;
            binary->right = factor();
            expr = binary;
        }
        return expr;
    }

    ExprPtr Parser::factor() {
        ExprPtr expr = power();
        while (match(TokenType::STAR) || match(TokenType::SLASH) || match(TokenType::PERCENT)) {
            auto binary = std::make_shared<BinaryExpr>();
            binary->left = expr;
            binary->op = tokens[pos - 1].type;
            binary->right = power();
            expr = binary;
        }
        return expr;
    }

    ExprPtr Parser::power() {
        ExprPtr expr = unary();
        if (match(TokenType::POWER)) {
            auto binary = std::make_shared<BinaryExpr>();
            binary->left = expr;
            binary->op = TokenType::POWER;
            binary->right = power();
            return binary;
        }
        return expr;
    }

    ExprPtr Parser::unary() {
        if (match(TokenType::MINUS) || match(TokenType::NOT)) {
            auto unary = std::make_shared<UnaryExpr>();
            unary->op = tokens[pos - 1].type;
            unary->operand = postfix();
            return unary;
        }
        return postfix();
    }

    ExprPtr Parser::postfix() {
        ExprPtr expr = primary();
        while (true) {
            if (match(TokenType::LPAREN)) {
                auto call = std::make_shared<CallExpr>();
                call->callee = expr;
                if (!check(TokenType::RPAREN)) {
                    do {
                        call->arguments.push_back(expression());
                    } while (match(TokenType::COMMA));
                }
                consume(TokenType::RPAREN, "Expected ')' after arguments");
                expr = call;
            } else if (match(TokenType::LBRACKET)) {
                auto index = std::make_shared<IndexExpr>();
                index->object = expr;
                index->index = expression();
                consume(TokenType::RBRACKET, "Expected ']' after index");
                expr = index;
            } else if (match(TokenType::DOT)) {
                auto member = std::make_shared<MemberExpr>();
                member->object = expr;
                member->member = consume(TokenType::IDENTIFIER, "Expected member name").lexeme;
                expr = member;
            } else {
                break;
            }
        }
        return expr;
    }

    ExprPtr Parser::primary() {
        if (match(TokenType::NUMBER)) {
            auto expr = std::make_shared<NumberExpr>();
            expr->value = std::stod(tokens[pos - 1].lexeme);
            return expr;
        }
        if (match(TokenType::STRING)) {
            auto expr = std::make_shared<StringExpr>();
            expr->value = tokens[pos - 1].lexeme;
            return expr;
        }
        if (match(TokenType::BOOLEAN)) {
            auto expr = std::make_shared<BooleanExpr>();
            expr->value = tokens[pos - 1].lexeme == "صحيح" || tokens[pos - 1].lexeme == "true";
            return expr;
        }
        if (match(TokenType::NIL)) {
            return std::make_shared<NilExpr>();
        }
        if (match(TokenType::IDENTIFIER)) {
            auto expr = std::make_shared<IdentifierExpr>();
            expr->name = tokens[pos - 1].lexeme;
            return expr;
        }
        if (match(TokenType::LPAREN)) {
            ExprPtr expr = expression();
            consume(TokenType::RPAREN, "Expected ')' after expression");
            return expr;
        }
        if (match(TokenType::LBRACKET)) {
            auto expr = std::make_shared<ArrayExpr>();
            if (!check(TokenType::RBRACKET)) {
                do {
                    expr->elements.push_back(expression());
                } while (match(TokenType::COMMA));
            }
            consume(TokenType::RBRACKET, "Expected ']' after array elements");
            return expr;
        }
        throw std::runtime_error("Unexpected token");
    }
}
