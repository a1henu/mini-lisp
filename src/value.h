#ifndef VALUE_H
#define VALUE_H

#include <memory>
#include <optional>
#include <string>
#include <vector>

class Value;
using ValuePtr = std::shared_ptr<Value>;

enum class ValueType {
    BOOLEAN,
    NUMERIC,
    STRING,
    NIL,
    SYMBOL,
    PAIR,
    BUILTIN,
};

class Value {
    ValueType type;

public:
    Value(ValueType type) : type{type} {}

    ValueType getType() const;

    virtual std::string toString() const;
    virtual std::vector<ValuePtr> toVector() const;

    bool isType(ValueType type) const;

    virtual std::optional<bool> asBoolean() const;
    virtual std::optional<double> asNumber() const;
    virtual std::optional<std::string> asString() const;
    virtual std::optional<std::string> asSymbol() const;
};


class BooleanValue : public Value {
    bool value;

public:
    BooleanValue(bool value) : Value(ValueType::BOOLEAN), value{value} {}

    std::string toString() const override;

    std::optional<bool> asBoolean() const override;
};


class NumericValue : public Value {
    double value;

public:
    NumericValue(double value) : Value(ValueType::NUMERIC), value{value} {}

    std::string toString() const override;

    std::optional<double> asNumber() const override;
};

class StringValue : public Value {
    std::string value;

public:
    StringValue(const std::string& value) : Value(ValueType::STRING), value{value} {}

    std::string toString() const override;

    std::optional<std::string> asString() const override;
};


class NilValue : public Value {
public:
    NilValue() : Value(ValueType::NIL) {}

    std::string toString() const override;
    std::vector<ValuePtr> toVector() const override;
};


class SymbolValue : public Value {
    std::string value;

public:
    SymbolValue(const std::string& value) : Value(ValueType::SYMBOL), value{value} {}

    std::string toString() const override;

    std::optional<std::string> asSymbol() const override;
};


class PairValue : public Value {
    ValuePtr car;
    ValuePtr cdr;

public:
    PairValue(ValuePtr car, ValuePtr cdr) : Value(ValueType::PAIR), car{car}, cdr{cdr} {}
    PairValue(const std::vector<ValuePtr>& values);

    std::string toString() const override;
    std::vector<ValuePtr> toVector() const override;

    ValuePtr getCar() const;
    ValuePtr getCdr() const;
};


class BuiltinProcValue : public Value {
    using BuiltinFuncType = ValuePtr(const std::vector<ValuePtr>&);
    BuiltinFuncType* func;

public:
    BuiltinProcValue(BuiltinFuncType* func) : Value(ValueType::BUILTIN), func{func} {}

    ValuePtr call(const std::vector<ValuePtr>& args) const;

    std::string toString() const override;
};

#endif