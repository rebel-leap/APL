#include "apl/compiler.h"
#include <iomanip>

namespace apl {
    String Compiler::compile(const Vector<StmtPtr>& statements) {
        out.str("");
        out << "#include <iostream>\n";
        out << "#include <string>\n";
        out << "#include <vector>\n";
        out << "#include <cmath>\n";
        out << "#include <cstdlib>\n";
        out << "\nusing namespace std;\n\n";
        for (const auto& stmt : statements) compileStmt(stmt);
        out << "int main() {\n    return 0;\n}\n";
        return out.str();
    }

    void Compiler::emit(const String& code) { out << code; }
    void Compiler::emitLine(const String& code) { for (int i = 0; i < indentLevel; i++) out << "    "; out << code << "\n"; }
    void Compiler::indent() { indentLevel++; }
    void Compiler::dedent() { indentLevel--; }

    void Compiler::compileStmt(StmtPtr stmt) {
        if (auto varStmt = std::dynamic_pointer_cast<VarDeclStmt>(stmt)) {
            emitLine("auto " + varStmt->name + " = " + exprToCpp(varStmt->initializer) + ";");
        } else if (auto exprStmt = std::dynamic_pointer_cast<ExpressionStmt>(stmt)) {
            emitLine(exprToCpp(exprStmt->expression) + ";");
        } else if (auto blockStmt = std::dynamic_pointer_cast<BlockStmt>(stmt)) {
            emitLine("{"); indent();
            for (const auto& s : blockStmt->statements) compileStmt(s);
            dedent(); emitLine("}");
        } else if (auto ifStmt = std::dynamic_pointer_cast<IfStmt>(stmt)) {
            emitLine("if (" + exprToCpp(ifStmt->condition) + ") {"); indent();
            compileStmt(ifStmt->thenBranch); dedent();
            if (ifStmt->elseBranch) { emitLine("} else {"); indent(); compileStmt(ifStmt->elseBranch); dedent(); }
            emitLine("}");
        } else if (auto whileStmt = std::dynamic_pointer_cast<WhileStmt>(stmt)) {
            emitLine("while (" + exprToCpp(whileStmt->condition) + ") {"); indent();
            compileStmt(whileStmt->body); dedent(); emitLine("}");
        } else if (auto funcStmt = std::dynamic_pointer_cast<FuncDeclStmt>(stmt)) {
            emit("auto " + funcStmt->name + " = [](");
            for (size_t i = 0; i < funcStmt->params.size(); i++) { if (i > 0) emit(", "); emit("auto " + funcStmt->params[i]); }
            emitLine(") {"); indent(); compileStmt(funcStmt->body); dedent(); emitLine("};");
        } else if (auto returnStmt = std::dynamic_pointer_cast<ReturnStmt>(stmt)) {
            if (returnStmt->value) emitLine("return " + exprToCpp(returnStmt->value) + ";");
            else emitLine("return;");
        }
    }

    void Compiler::compileExpr(ExprPtr expr) { emit(exprToCpp(expr)); }

    String Compiler::exprToCpp(ExprPtr expr) {
        if (auto numExpr = std::dynamic_pointer_cast<NumberExpr>(expr)) return std::to_string(numExpr->value);
        if (auto strExpr = std::dynamic_pointer_cast<StringExpr>(expr)) {
            String result = "\"";
            for (char c : strExpr->value) {
                switch (c) {
                    case '\\': result += "\\\\"; break;
                    case '"': result += "\\\""; break;
                    case '\n': result += "\\n"; break;
                    case '\t': result += "\\t"; break;
                    case '\r': result += "\\r"; break;
                    default: result += c; break;
                }
            }
            result += "\""; return result;
        }
        if (auto boolExpr = std::dynamic_pointer_cast<BooleanExpr>(expr)) return boolExpr->value ? "true" : "false";
        if (std::dynamic_pointer_cast<NilExpr>(expr)) return "nullptr";
        if (auto idExpr = std::dynamic_pointer_cast<IdentifierExpr>(expr)) return idExpr->name;
        if (auto arrExpr = std::dynamic_pointer_cast<ArrayExpr>(expr)) {
            String result = "{";
            for (size_t i = 0; i < arrExpr->elements.size(); i++) { if (i > 0) result += ", "; result += exprToCpp(arrExpr->elements[i]); }
            result += "}"; return result;
        }
        if (auto binary = std::dynamic_pointer_cast<BinaryExpr>(expr)) {
            String op;
            switch (binary->op) {
                case TokenType::PLUS: op = "+"; break;
                case TokenType::MINUS: op = "-"; break;
                case TokenType::STAR: op = "*"; break;
                case TokenType::SLASH: op = "/"; break;
                case TokenType::PERCENT: op = "%"; break;
                case TokenType::POWER: op = "pow"; break;
                case TokenType::EQ: op = "=="; break;
                case TokenType::NE: op = "!="; break;
                case TokenType::LT: op = "<"; break;
                case TokenType::GT: op = ">"; break;
                case TokenType::LE: op = "<="; break;
                case TokenType::GE: op = ">="; break;
                case TokenType::AND: op = "&&"; break;
                case TokenType::OR: op = "||"; break;
                default: op = "+"; break;
            }
            if (binary->op == TokenType::POWER) return "std::pow(" + exprToCpp(binary->left) + ", " + exprToCpp(binary->right) + ")";
            return "(" + exprToCpp(binary->left) + " " + op + " " + exprToCpp(binary->right) + ")";
        }
        if (auto unary = std::dynamic_pointer_cast<UnaryExpr>(expr)) {
            String op = (unary->op == TokenType::MINUS) ? "-" : "!";
            return op + exprToCpp(unary->operand);
        }
        if (auto call = std::dynamic_pointer_cast<CallExpr>(expr)) {
            String result = exprToCpp(call->callee) + "(";
            for (size_t i = 0; i < call->arguments.size(); i++) { if (i > 0) result += ", "; result += exprToCpp(call->arguments[i]); }
            result += ")"; return result;
        }
        if (auto index = std::dynamic_pointer_cast<IndexExpr>(expr)) return exprToCpp(index->object) + "[" + exprToCpp(index->index) + "]";
        if (auto member = std::dynamic_pointer_cast<MemberExpr>(expr)) return exprToCpp(member->object) + "." + member->member;
        if (auto assign = std::dynamic_pointer_cast<AssignExpr>(expr)) return exprToCpp(assign->target) + " = " + exprToCpp(assign->value);
        return "0";
    }

    String Compiler::stmtToCpp(StmtPtr stmt) {
        std::ostringstream temp;
        auto* oldPtr = &out;
        out.str("");
        compileStmt(stmt);
        String result = out.str();
        out.str(temp.str());
        return result;
    }
}
