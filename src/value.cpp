#include <cmath>
#include <iomanip>
#include <sstream>

#include "./error.h"
#include "./eval_env.h"
#include "./value.h"

/**Value class
 * Methods for base class Value 
 */
ValueType Value::getType() const {
    return type;
}

std::string Value::toString() const {
    return "";
}

std::vector<ValuePtr> Value::toVector() const {
    throw LispError("Cannot convert to vector");
}

bool Value::isType(ValueType type) const {
    return this->type == type;
}

bool Value::asBoolean() const {
    return true;
}

std::optional<double> Value::asNumber() const {
    return std::nullopt;
}

std::optional<std::string> Value::asString() const {
    return std::nullopt;
}

std::optional<std::string> Value::asSymbol() const {
    return std::nullopt;
}

ValuePtr Value::call(const std::vector<ValuePtr>& args, EvalEnv& env) const {
    throw LispError("Cannot call this value");
}


/**BooleanValue class
 * Methods for derived class BooleanValue 
 */
std::string BooleanValue::toString() const {
    return value ? "#t" : "#f";
}

bool BooleanValue::asBoolean() const {
    return value;
}


/**NumericValue class
 * Methods for derived class NumericValue 
 */
std::string NumericValue::toString() const {
    if (std::floor(value) == value) {
        return std::to_string(static_cast<int>(value));
    } else {
        return std::to_string(value);
    }
}

std::optional<double> NumericValue::asNumber() const {
    return value;
}


/**StringValue class
 * Methods for derived class StringValue
 */
std::string StringValue::toString() const {
    std::stringstream ss;
    ss << std::quoted(value);
    return ss.str();
}

std::optional<std::string> StringValue::asString() const {
    return value;
}


/**NilValue class
 * Methods for derived class NilValue
 */
std::string NilValue::toString() const {
    return "()";
}

std::vector<ValuePtr> NilValue::toVector() const {
    return {};
}


/**SymbolValue class
 * Methods for derived class SymbolValue
 */
std::string SymbolValue::toString() const {
    return value;
}

std::optional<std::string> SymbolValue::asSymbol() const {
    return value;
}


/**PairValue class
 * Methods for derived class PairValue
 */
PairValue::PairValue(const std::vector<ValuePtr>& values) : Value(ValueType::PAIR) {
    if (values.empty()) {
        throw SyntaxError("Cannot create a pair from empty list");
    }
    car = values[0];
    if (values.size() == 1) {
        cdr = std::make_shared<NilValue>();
    } else {
        cdr = std::make_shared<PairValue>(std::vector<ValuePtr>(values.begin() + 1, values.end()));
    }
}

std::string PairValue::toString() const {
    std::stringstream ss;
    ss << "(";

    auto cur = this;
    while(cur) {
        ss << cur->car->toString();
        if (cur->cdr->isType(ValueType::NIL)) {
            cur = nullptr;
        } else if (cur->cdr->isType(ValueType::PAIR)) {
            ss << " ";
            cur = static_cast<PairValue*>(cur->cdr.get());
        } else {
            ss << " . " << cur->cdr->toString();
            cur = nullptr;
        }
    }

    ss << ")";
    return ss.str();
}

std::vector<ValuePtr> PairValue::toVector() const {
    std::vector<ValuePtr> result;
    auto cur = this;
    while(cur) {
        result.push_back(cur->car);
        if (cur->cdr->isType(ValueType::NIL)) {
            cur = nullptr;
        } else if (cur->cdr->isType(ValueType::PAIR)) {
            cur = static_cast<PairValue*>(cur->cdr.get());
        } else {
            result.push_back(cur->cdr);
            cur = nullptr;
        }
    }
    return result;
}

ValuePtr PairValue::getCar() const {
    return car;
}

ValuePtr PairValue::getCdr() const {
    return cdr;
}


/**BuiltinProcValue class
 * Methods for derived class BuiltinProcValu
 */
ValuePtr BuiltinProcValue::call(const std::vector<ValuePtr>& args, EvalEnv& env) const {
    return func(args, env);
}

std::string BuiltinProcValue::toString() const {
    return "#<procedure>";
}


/**LambdaValue class
 * Methods for derived class LambdaValue 
 */
ValuePtr LambdaValue::call(const std::vector<ValuePtr>& args, EvalEnv& env) const {
    auto childEnv = this->env->createChild(params, args);
    ValuePtr lastValue;
    for (const auto& expr : body) {
        lastValue = childEnv->eval(expr);
    }
    return lastValue;
}

std::string LambdaValue::toString() const {
    return "#<procedure>";
}