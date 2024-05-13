#ifndef EVAL_ENV_H
#define EVAL_ENV_H

#include <algorithm>
#include <iterator>
#include <unordered_map>

#include "value.h"

class EvalEnv {
    std::unordered_map<std::string, ValuePtr> SYMBOL_TABLE;
public:
    EvalEnv();

    ValuePtr apply(ValuePtr proc, std::vector<ValuePtr> args);

    ValuePtr eval(ValuePtr expr);
    std::vector<ValuePtr> evalList(ValuePtr expr);

    void define(const std::string& symbol, ValuePtr value);
    ValuePtr lookup(const std::string& symbol);
};

#endif