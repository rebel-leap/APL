#include "apl/env.h"
#include <stdexcept>

namespace apl {
    Environment::Environment(std::shared_ptr<Environment> enclosing) : enclosing(enclosing) {}

    void Environment::define(const String& name, const Value& value) {
        values[name] = value;
    }

    Value Environment::get(const String& name) {
        auto it = values.find(name);
        if (it != values.end()) return it->second;
        if (enclosing) return enclosing->get(name);
        throw std::runtime_error("Undefined variable: " + name);
    }

    void Environment::assign(const String& name, const Value& value) {
        auto it = values.find(name);
        if (it != values.end()) { it->second = value; return; }
        if (enclosing) { enclosing->assign(name, value); return; }
        throw std::runtime_error("Undefined variable: " + name);
    }

    bool Environment::exists(const String& name) const {
        if (values.find(name) != values.end()) return true;
        if (enclosing) return enclosing->exists(name);
        return false;
    }
}
