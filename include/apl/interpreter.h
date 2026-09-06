#pragma once
#include "apl/env.h"
#include "apl/ast.h"  // Added: for StmtPtr and ExprPtr

namespace apl {
    class Interpreter {
    public:
        Interpreter();
        void execute(const Vector<StmtPtr>& statements);
        Value evaluate(ExprPtr expr);
        Value callFunction(const Value& callee, const Vector<Value>& arguments);
        std::shared_ptr<Environment> globals;
        std::shared_ptr<Environment> environment;
    private:
        void executeStmt(StmtPtr stmt);
        Value evaluateExpr(ExprPtr expr);
        void defineNative(const String& name, NativeFunc func);
    };
}
