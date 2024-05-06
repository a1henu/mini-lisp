#ifndef EVAL_ENV_H
#define EVAL_ENV_H

#include <unordered_map>

#include "value.h"

class EvalEnv {
    std::unordered_map<std::string, ValuePtr> SYMBOL_TABLE;
public:
    ValuePtr eval(ValuePtr expr);

    void define(const std::string& symbol, ValuePtr value);
    ValuePtr lookup(const std::string& symbol);
};

#endif