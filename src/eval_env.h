#ifndef EVAL_ENV_H
#define EVAL_ENV_H

#include <algorithm>
#include <iterator>
#include <unordered_map>

#include "value.h"

class EvalEnv : public std::enable_shared_from_this<EvalEnv> {
    std::unordered_map<std::string, ValuePtr> SYMBOL_TABLE;
    std::shared_ptr<EvalEnv> parent;

public:
    EvalEnv();
    EvalEnv(std::shared_ptr<EvalEnv> parent);

    std::shared_ptr<EvalEnv> createChild(const std::vector<std::string>& params, const std::vector<ValuePtr>& args);

    ValuePtr apply(ValuePtr proc, std::vector<ValuePtr> args);
    ValuePtr eval(ValuePtr expr);
    std::vector<ValuePtr> evalList(ValuePtr expr);

    void define(const std::string& symbol, ValuePtr value);
    ValuePtr lookup(const std::string& symbol);
};

#endif