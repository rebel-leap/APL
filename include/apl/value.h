#pragma once
#include "apl/common.h"

namespace apl {
    enum class ValueType { NUMBER, STRING, BOOLEAN, NIL, ARRAY, FUNCTION, OBJECT, NATIVE };

    struct Value {
        ValueType type;
        double numberValue = 0;
        String stringValue;
        bool boolValue = false;
        Vector<Value> arrayValue;
        std::shared_ptr<void> objectValue;

        Value() : type(ValueType::NIL) {}
        explicit Value(double n) : type(ValueType::NUMBER), numberValue(n) {}
        explicit Value(const String& s) : type(ValueType::STRING), stringValue(s) {}
        explicit Value(bool b) : type(ValueType::BOOLEAN), boolValue(b) {}
        explicit Value(const Vector<Value>& a) : type(ValueType::ARRAY), arrayValue(a) {}

        static Value makeNumber(double n);
        static Value makeString(const String& s);
        static Value makeBoolean(bool b);
        static Value makeNil();
        static Value makeArray(const Vector<Value>& a);

        String toString() const;
        bool isTruthy() const;
        bool isEqual(const Value& other) const;
        bool operator==(const Value& other) const;  // Added
    };

    using NativeFunc = Value(*)(const Vector<Value>&);
}
