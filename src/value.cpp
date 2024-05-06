#include <cmath>
#include <iomanip>
#include <sstream>

#include "./error.h"
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

std::optional<std::string> Value::asSymbol() const {
    return std::nullopt;
}


/**BooleanValue class
 * Methods for derived class BooleanValue 
 */
std::string BooleanValue::toString() const {
    return value ? "#t" : "#f";
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


/**StringValue class
 * Methods for derived class StringValue
 */
std::string StringValue::toString() const {
    std::stringstream ss;
    ss << std::quoted(value);
    return ss.str();
}


/**NilValue class
 * Methods for derived class NilValue
 */
std::string NilValue::toString() const {
    return "()";
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
        if (cur->cdr->getType() == ValueType::NIL) {
            cur = nullptr;
        } else if (cur->cdr->getType() == ValueType::PAIR) {
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
        if (cur->cdr->getType() == ValueType::NIL) {
            cur = nullptr;
        } else if (cur->cdr->getType() == ValueType::PAIR) {
            cur = static_cast<PairValue*>(cur->cdr.get());
        } else {
            result.push_back(cur->cdr);
            cur = nullptr;
        }
    }
    return result;
}