#pragma once
#include "apl/common.h"
#include "apl/lexer.h"

namespace apl {
    struct Expr { virtual ~Expr() = default; };
    struct Stmt { virtual ~Stmt() = default; };
    using ExprPtr = std::shared_ptr<Expr>;
    using StmtPtr = std::shared_ptr<Stmt>;

    struct NumberExpr : Expr { double value; };
    struct StringExpr : Expr { String value; };
    struct BooleanExpr : Expr { bool value; };
    struct NilExpr : Expr {};
    struct IdentifierExpr : Expr { String name; };
    struct BinaryExpr : Expr { ExprPtr left; TokenType op; ExprPtr right; };
    struct UnaryExpr : Expr { TokenType op; ExprPtr operand; };
    struct CallExpr : Expr { ExprPtr callee; Vector<ExprPtr> arguments; };
    struct IndexExpr : Expr { ExprPtr object; ExprPtr index; };
    struct ArrayExpr : Expr { Vector<ExprPtr> elements; };
    struct MemberExpr : Expr { ExprPtr object; String member; };
    struct AssignExpr : Expr { ExprPtr target; ExprPtr value; };

    struct ExpressionStmt : Stmt { ExprPtr expression; };
    struct VarDeclStmt : Stmt { String name; ExprPtr initializer; };
    struct BlockStmt : Stmt { Vector<StmtPtr> statements; };
    struct IfStmt : Stmt { ExprPtr condition; StmtPtr thenBranch; StmtPtr elseBranch; };
    struct WhileStmt : Stmt { ExprPtr condition; StmtPtr body; };
    struct ForStmt : Stmt { String varName; ExprPtr iterable; StmtPtr body; };
    struct ReturnStmt : Stmt { ExprPtr value; };
    struct FuncDeclStmt : Stmt { String name; Vector<String> params; StmtPtr body; };
    struct ClassDeclStmt : Stmt { String name; Vector<std::pair<String, FuncDeclStmt>> methods; };
    struct ImportStmt : Stmt { String path; };
}
