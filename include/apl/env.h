#pragma once
#include "apl/value.h"

namespace apl {
    class Environment : public std::enable_shared_from_this<Environment> {
    public:
        Environment() = default;
        explicit Environment(std::shared_ptr<Environment> enclosing);
        void define(const String& name, const Value& value);
        Value get(const String& name);
        void assign(const String& name, const Value& value);
        bool exists(const String& name) const;
    private:
        Map<String, Value> values;
        std::shared_ptr<Environment> enclosing;
    };
}
