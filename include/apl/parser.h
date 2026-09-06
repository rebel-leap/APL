#pragma once
#include "apl/ast.h"

namespace apl {
    class Parser {
    public:
        explicit Parser(const Vector<Token>& tokens);
        Vector<StmtPtr> parse();
    private:
        Vector<Token> tokens;
        size_t pos = 0;
        Token& peek(size_t offset = 0);
        Token& advance();
        bool match(TokenType type);
        bool check(TokenType type);
        Token& consume(TokenType type, const String& message);
        StmtPtr declaration();
        StmtPtr statement();
        StmtPtr varDeclaration();
        StmtPtr funcDeclaration();
        StmtPtr classDeclaration();
        StmtPtr ifStatement();
        StmtPtr whileStatement();
        StmtPtr forStatement();
        StmtPtr returnStatement();
        StmtPtr importStatement();
        StmtPtr expressionStatement();
        StmtPtr blockStatement();
        ExprPtr expression();
        ExprPtr assignment();
        ExprPtr orExpr();
        ExprPtr andExpr();
        ExprPtr equality();
        ExprPtr comparison();
        ExprPtr term();
        ExprPtr factor();
        ExprPtr power();
        ExprPtr unary();
        ExprPtr postfix();
        ExprPtr primary();
        bool isAtEnd();
    };
}
