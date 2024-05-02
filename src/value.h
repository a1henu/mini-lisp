#ifndef VALUE_H
#define VALUE_H

#include <memory>
#include <string>

class Value;
using ValuePtr = std::shared_ptr<Value>;

enum class ValueType {
    BOOLEAN,
    NUMERIC,
    STRING,
    NIL,
    SYMBOL,
    PAIR,
};

class Value {
    ValueType type;

public:
    Value(ValueType type) : type{type} {}

    ValueType getType() const;

    virtual std::string toString() const;
};


class BooleanValue : public Value {
    bool value;

public:
    BooleanValue(bool value) : Value(ValueType::BOOLEAN), value{value} {}

    std::string toString() const override;
};


class NumericValue : public Value {
    double value;

public:
    NumericValue(double value) : Value(ValueType::NUMERIC), value{value} {}

    std::string toString() const override;
};

class StringValue : public Value {
    std::string value;

public:
    StringValue(const std::string& value) : Value(ValueType::STRING), value{value} {}

    std::string toString() const override;
};


class NilValue : public Value {
public:
    NilValue() : Value(ValueType::NIL) {}

    std::string toString() const override;
};


class SymbolValue : public Value {
    std::string value;

public:
    SymbolValue(const std::string& value) : Value(ValueType::SYMBOL), value{value} {}

    std::string toString() const override;
};


class PairValue : public Value {
    ValuePtr car;
    ValuePtr cdr;

public:
    PairValue(ValuePtr car, ValuePtr cdr) : Value(ValueType::PAIR), car{car}, cdr{cdr} {}

    std::string toString() const override;
};


#endif