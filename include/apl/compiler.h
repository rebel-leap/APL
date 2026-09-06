#pragma once
#include "apl/ast.h"

namespace apl {
    class Compiler {
    public:
        String compile(const Vector<StmtPtr>& statements);
    private:
        std::ostringstream out;
        int indentLevel = 0;
        void emit(const String& code);
        void emitLine(const String& code);
        void indent();
        void dedent();
        void compileStmt(StmtPtr stmt);
        void compileExpr(ExprPtr expr);
        String exprToCpp(ExprPtr expr);
        String stmtToCpp(StmtPtr stmt);
    };
}
