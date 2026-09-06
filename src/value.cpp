#include "apl/value.h"
#include <sstream>
#include <iomanip>

namespace apl {
    Value Value::makeNumber(double n) { return Value(n); }
    Value Value::makeString(const String& s) { return Value(s); }
    Value Value::makeBoolean(bool b) { return Value(b); }
    Value Value::makeNil() { return Value(); }
    Value Value::makeArray(const Vector<Value>& a) { return Value(a); }

    String Value::toString() const {
        switch (type) {
            case ValueType::NUMBER: { std::ostringstream oss; oss << numberValue; return oss.str(); }
            case ValueType::STRING: return stringValue;
            case ValueType::BOOLEAN: return boolValue ? "صحيح" : "خطأ";
            case ValueType::NIL: return "عدم";
            case ValueType::ARRAY: {
                String result = "[";
                for (size_t i = 0; i < arrayValue.size(); i++) {
                    if (i > 0) result += ", ";
                    result += arrayValue[i].toString();
                }
                result += "]";
                return result;
            }
            default: return "<object>";
        }
    }

    bool Value::isTruthy() const {
        switch (type) {
            case ValueType::NIL: return false;
            case ValueType::BOOLEAN: return boolValue;
            case ValueType::NUMBER: return numberValue != 0;
            case ValueType::STRING: return !stringValue.empty();
            case ValueType::ARRAY: return !arrayValue.empty();
            default: return true;
        }
    }

    bool Value::isEqual(const Value& other) const {
        if (type != other.type) return false;
        switch (type) {
            case ValueType::NUMBER: return numberValue == other.numberValue;
            case ValueType::STRING: return stringValue == other.stringValue;
            case ValueType::BOOLEAN: return boolValue == other.boolValue;
            case ValueType::NIL: return true;
            case ValueType::ARRAY: return arrayValue == other.arrayValue;
            default: return objectValue == other.objectValue;
        }
    }

    bool Value::operator==(const Value& other) const {
        return isEqual(other);
    }
}
