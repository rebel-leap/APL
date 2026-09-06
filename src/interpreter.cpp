#include "apl/interpreter.h"
#include <cmath>
#include <iostream>

namespace apl {
    static Value printNative(const Vector<Value>& args) {
        for (const auto& arg : args) std::cout << arg.toString();
        return Value::makeNil();
    }
    static Value printlnNative(const Vector<Value>& args) {
        for (const auto& arg : args) std::cout << arg.toString();
        std::cout << std::endl;
        return Value::makeNil();
    }
    static Value inputNative(const Vector<Value>& args) {
        String prompt = args.empty() ? "" : args[0].toString();
        std::cout << prompt;
        String line;
        std::getline(std::cin, line);
        return Value::makeString(line);
    }
    static Value lenNative(const Vector<Value>& args) {
        if (args.empty()) return Value::makeNumber(0);
        if (args[0].type == ValueType::STRING) return Value::makeNumber(static_cast<double>(args[0].stringValue.size()));
        if (args[0].type == ValueType::ARRAY) return Value::makeNumber(static_cast<double>(args[0].arrayValue.size()));
        return Value::makeNumber(0);
    }
    static Value typeNative(const Vector<Value>& args) {
        if (args.empty()) return Value::makeString("عدم");
        switch (args[0].type) {
            case ValueType::NUMBER: return Value::makeString("رقم");
            case ValueType::STRING: return Value::makeString("نص");
            case ValueType::BOOLEAN: return Value::makeString("منطقي");
            case ValueType::NIL: return Value::makeString("عدم");
            case ValueType::ARRAY: return Value::makeString("مصفوفة");
            default: return Value::makeString("كائن");
        }
    }
    static Value toNumberNative(const Vector<Value>& args) {
        if (args.empty()) return Value::makeNumber(0);
        try { return Value::makeNumber(std::stod(args[0].toString())); } catch (...) { return Value::makeNumber(0); }
    }
    static Value sqrtNative(const Vector<Value>& args) {
        if (args.empty()) return Value::makeNumber(0);
        return Value::makeNumber(std::sqrt(args[0].numberValue));
    }
    static Value powNative(const Vector<Value>& args) {
        if (args.size() < 2) return Value::makeNumber(0);
        return Value::makeNumber(std::pow(args[0].numberValue, args[1].numberValue));
    }
    static Value sinNative(const Vector<Value>& args) {
        if (args.empty()) return Value::makeNumber(0);
        return Value::makeNumber(std::sin(args[0].numberValue));
    }
    static Value cosNative(const Vector<Value>& args) {
        if (args.empty()) return Value::makeNumber(0);
        return Value::makeNumber(std::cos(args[0].numberValue));
    }
    static Value randomNative(const Vector<Value>& args) {
        return Value::makeNumber(static_cast<double>(rand()) / RAND_MAX);
    }
    static Value floorNative(const Vector<Value>& args) {
        if (args.empty()) return Value::makeNumber(0);
        return Value::makeNumber(std::floor(args[0].numberValue));
    }
    static Value ceilNative(const Vector<Value>& args) {
        if (args.empty()) return Value::makeNumber(0);
        return Value::makeNumber(std::ceil(args[0].numberValue));
    }
    static Value absNative(const Vector<Value>& args) {
        if (args.empty()) return Value::makeNumber(0);
        return Value::makeNumber(std::abs(args[0].numberValue));
    }
    static Value minNative(const Vector<Value>& args) {
        if (args.empty()) return Value::makeNumber(0);
        double m = args[0].numberValue;
        for (const auto& arg : args) if (arg.numberValue < m) m = arg.numberValue;
        return Value::makeNumber(m);
    }
    static Value maxNative(const Vector<Value>& args) {
        if (args.empty()) return Value::makeNumber(0);
        double m = args[0].numberValue;
        for (const auto& arg : args) if (arg.numberValue > m) m = arg.numberValue;
        return Value::makeNumber(m);
    }
    static Value pushNative(const Vector<Value>& args) {
        if (args.size() < 2 || args[0].type != ValueType::ARRAY) return Value::makeNil();
        Value arr = args[0];
        arr.arrayValue.push_back(args[1]);
        return arr;
    }
    static Value popNative(const Vector<Value>& args) {
        if (args.empty() || args[0].type != ValueType::ARRAY) return Value::makeNil();
        Value arr = args[0];
        if (!arr.arrayValue.empty()) arr.arrayValue.pop_back();
        return arr;
    }
    static Value rangeNative(const Vector<Value>& args) {
        if (args.size() < 2) return Value::makeArray({});
        double start = args[0].numberValue;
        double end = args[1].numberValue;
        double step = args.size() > 2 ? args[2].numberValue : 1;
        Vector<Value> result;
        if (step > 0) { for (double i = start; i < end; i += step) result.push_back(Value::makeNumber(i)); }
        else { for (double i = start; i > end; i += step) result.push_back(Value::makeNumber(i)); }
        return Value::makeArray(result);
    }
    static Value clockNative(const Vector<Value>& args) {
        return Value::makeNumber(static_cast<double>(clock()) / CLOCKS_PER_SEC);
    }

    Interpreter::Interpreter() {
        globals = std::make_shared<Environment>();
        environment = globals;
        defineNative("اطبع", printNative);
        defineNative("اطبع_سطر", printlnNative);
        defineNative("ادخل", inputNative);
        defineNative("الطول", lenNative);
        defineNative("النوع", typeNative);
        defineNative("لرقم", toNumberNative);
        defineNative("جذر", sqrtNative);
        defineNative("قوة", powNative);
        defineNative("جيب", sinNative);
        defineNative("جيب_تمام", cosNative);
        defineNative("عشوائي", randomNative);
        defineNative("اقرب_صغير", floorNative);
        defineNative("اقرب_كبير", ceilNative);
        defineNative("مطلق", absNative);
        defineNative("الأصغر", minNative);
        defineNative("الأكبر", maxNative);
        defineNative("أضف", pushNative);
        defineNative("أزل", popNative);
        defineNative("مجال", rangeNative);
        defineNative("الوقت", clockNative);
    }

    void Interpreter::defineNative(const String& name, NativeFunc func) {
        Value val;
        val.type = ValueType::NATIVE;
        val.objectValue = std::shared_ptr<void>(reinterpret_cast<void*>(func), [](void*){});
        globals->define(name, val);
    }

    void Interpreter::execute(const Vector<StmtPtr>& statements) {
        for (const auto& stmt : statements) executeStmt(stmt);
    }

    void Interpreter::executeStmt(StmtPtr stmt) {
        if (auto exprStmt = std::dynamic_pointer_cast<ExpressionStmt>(stmt)) {
            evaluateExpr(exprStmt->expression);
        } else if (auto varStmt = std::dynamic_pointer_cast<VarDeclStmt>(stmt)) {
            Value value = varStmt->initializer ? evaluateExpr(varStmt->initializer) : Value::makeNil();
            environment->define(varStmt->name, value);
        } else if (auto blockStmt = std::dynamic_pointer_cast<BlockStmt>(stmt)) {
            auto previous = environment;
            environment = std::make_shared<Environment>(environment);
            for (const auto& s : blockStmt->statements) executeStmt(s);
            environment = previous;
        } else if (auto ifStmt = std::dynamic_pointer_cast<IfStmt>(stmt)) {
            if (evaluateExpr(ifStmt->condition).isTruthy()) executeStmt(ifStmt->thenBranch);
            else if (ifStmt->elseBranch) executeStmt(ifStmt->elseBranch);
        } else if (auto whileStmt = std::dynamic_pointer_cast<WhileStmt>(stmt)) {
            while (evaluateExpr(whileStmt->condition).isTruthy()) executeStmt(whileStmt->body);
        } else if (auto forStmt = std::dynamic_pointer_cast<ForStmt>(stmt)) {
            auto iterable = evaluateExpr(forStmt->iterable);
            if (iterable.type == ValueType::ARRAY) {
                for (const auto& item : iterable.arrayValue) {
                    environment->define(forStmt->varName, item);
                    executeStmt(forStmt->body);
                }
            } else if (iterable.type == ValueType::STRING) {
                for (char c : iterable.stringValue) {
                    environment->define(forStmt->varName, Value::makeString(String(1, c)));
                    executeStmt(forStmt->body);
                }
            }
        } else if (auto returnStmt = std::dynamic_pointer_cast<ReturnStmt>(stmt)) {
            // Return handled by caller
        } else if (auto funcStmt = std::dynamic_pointer_cast<FuncDeclStmt>(stmt)) {
            Value funcVal;
            funcVal.type = ValueType::FUNCTION;
            funcVal.objectValue = std::make_shared<FuncDeclStmt>(*funcStmt);
            environment->define(funcStmt->name, funcVal);
        } else if (auto importStmt = std::dynamic_pointer_cast<ImportStmt>(stmt)) {
            // Import handled separately
        }
    }

    Value Interpreter::evaluate(ExprPtr expr) { return evaluateExpr(expr); }

    Value Interpreter::evaluateExpr(ExprPtr expr) {
        if (auto numExpr = std::dynamic_pointer_cast<NumberExpr>(expr)) return Value::makeNumber(numExpr->value);
        if (auto strExpr = std::dynamic_pointer_cast<StringExpr>(expr)) return Value::makeString(strExpr->value);
        if (auto boolExpr = std::dynamic_pointer_cast<BooleanExpr>(expr)) return Value::makeBoolean(boolExpr->value);
        if (std::dynamic_pointer_cast<NilExpr>(expr)) return Value::makeNil();
        if (auto idExpr = std::dynamic_pointer_cast<IdentifierExpr>(expr)) return environment->get(idExpr->name);
        if (auto arrExpr = std::dynamic_pointer_cast<ArrayExpr>(expr)) {
            Vector<Value> values;
            for (const auto& e : arrExpr->elements) values.push_back(evaluateExpr(e));
            return Value::makeArray(values);
        }
        if (auto binary = std::dynamic_pointer_cast<BinaryExpr>(expr)) {
            Value left = evaluateExpr(binary->left);
            Value right = evaluateExpr(binary->right);
            switch (binary->op) {
                case TokenType::PLUS:
                    if (left.type == ValueType::STRING || right.type == ValueType::STRING) return Value::makeString(left.toString() + right.toString());
                    return Value::makeNumber(left.numberValue + right.numberValue);
                case TokenType::MINUS: return Value::makeNumber(left.numberValue - right.numberValue);
                case TokenType::STAR: return Value::makeNumber(left.numberValue * right.numberValue);
                case TokenType::SLASH: if (right.numberValue == 0) throw std::runtime_error("Division by zero"); return Value::makeNumber(left.numberValue / right.numberValue);
                case TokenType::PERCENT: return Value::makeNumber(std::fmod(left.numberValue, right.numberValue));
                case TokenType::POWER: return Value::makeNumber(std::pow(left.numberValue, right.numberValue));
                case TokenType::EQ: return Value::makeBoolean(left.isEqual(right));
                case TokenType::NE: return Value::makeBoolean(!left.isEqual(right));
                case TokenType::LT: return Value::makeBoolean(left.numberValue < right.numberValue);
                case TokenType::GT: return Value::makeBoolean(left.numberValue > right.numberValue);
                case TokenType::LE: return Value::makeBoolean(left.numberValue <= right.numberValue);
                case TokenType::GE: return Value::makeBoolean(left.numberValue >= right.numberValue);
                case TokenType::AND: return Value::makeBoolean(left.isTruthy() && right.isTruthy());
                case TokenType::OR: return Value::makeBoolean(left.isTruthy() || right.isTruthy());
                default: return Value::makeNil();
            }
        }
        if (auto unary = std::dynamic_pointer_cast<UnaryExpr>(expr)) {
            Value operand = evaluateExpr(unary->operand);
            switch (unary->op) {
                case TokenType::MINUS: return Value::makeNumber(-operand.numberValue);
                case TokenType::NOT: return Value::makeBoolean(!operand.isTruthy());
                default: return Value::makeNil();
            }
        }
        if (auto call = std::dynamic_pointer_cast<CallExpr>(expr)) {
            Value callee = evaluateExpr(call->callee);
            Vector<Value> args;
            for (const auto& arg : call->arguments) args.push_back(evaluateExpr(arg));
            return callFunction(callee, args);
        }
        if (auto index = std::dynamic_pointer_cast<IndexExpr>(expr)) {
            Value object = evaluateExpr(index->object);
            Value idx = evaluateExpr(index->index);
            if (object.type == ValueType::ARRAY) {
                size_t i = static_cast<size_t>(idx.numberValue);
                if (i < object.arrayValue.size()) return object.arrayValue[i];
            } else if (object.type == ValueType::STRING) {
                size_t i = static_cast<size_t>(idx.numberValue);
                if (i < object.stringValue.size()) return Value::makeString(String(1, object.stringValue[i]));
            }
            return Value::makeNil();
        }
        if (auto member = std::dynamic_pointer_cast<MemberExpr>(expr)) return Value::makeNil();
        if (auto assign = std::dynamic_pointer_cast<AssignExpr>(expr)) {
            Value value = evaluateExpr(assign->value);
            if (auto id = std::dynamic_pointer_cast<IdentifierExpr>(assign->target)) environment->assign(id->name, value);
            return value;
        }
        return Value::makeNil();
    }

    Value Interpreter::callFunction(const Value& callee, const Vector<Value>& arguments) {
        if (callee.type == ValueType::NATIVE) {
            NativeFunc func = reinterpret_cast<NativeFunc>(callee.objectValue.get());
            return func(arguments);
        }
        return Value::makeNil();
    }
}
