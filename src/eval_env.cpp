#include "./error.h"
#include "./eval_env.h"

void EvalEnv::define(const std::string& symbol, ValuePtr value) {
    SYMBOL_TABLE[symbol] = value;
}

ValuePtr EvalEnv::lookup(const std::string& symbol) {
    if (auto it = SYMBOL_TABLE.find(symbol); it != SYMBOL_TABLE.end()) {
        return it->second;
    } else {
        throw LispError("Variable " + symbol + " not found.");
    }
}

bool isSelfEvaluating(ValuePtr expr) {
    return expr->getType() == ValueType::BOOLEAN || 
           expr->getType() == ValueType::NUMERIC || 
           expr->getType() == ValueType::STRING;
}

ValuePtr EvalEnv::eval(ValuePtr expr) {
    if (isSelfEvaluating(expr)) {
        return expr;
    } else if (expr->getType() == ValueType::NIL) {
        throw LispError("Evaluating nil is prohibited.");
    } else if (expr->getType() == ValueType::PAIR) {
        using namespace std::literals;
        auto exprs = expr->toVector();
        if (exprs[0]->asSymbol() == "define"s) {
            if (auto symbol = exprs[1]->asSymbol()) {
                define(*symbol, eval(exprs[2]));
                return std::make_shared<NilValue>();
            } else {
                throw LispError("Malformed define.");
            }
        } else {
            throw LispError("Unimplemented.");
        }
    } else if (expr->getType() == ValueType::SYMBOL) {
        if (auto symbol = expr->asSymbol()) {
            if (auto value = lookup(*symbol)) {
                return value;
            } else {
                throw LispError("Variable " + *symbol + " not defined.");
            }
        }
    }
    
    else {
        throw LispError("Unimplemented.");
    }
}