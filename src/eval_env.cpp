#include "./builtins.h"
#include "./error.h"
#include "./eval_env.h"
#include "./forms.h"

EvalEnv::EvalEnv() {
    builtins::init();
    for (const auto& [name, value] : builtins::builtins) {
        define(name, value);
    }
}

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
    return expr->isType(ValueType::BOOLEAN) || 
           expr->isType(ValueType::NUMERIC) || 
           expr->isType(ValueType::STRING);
}

ValuePtr EvalEnv::apply(ValuePtr proc, std::vector<ValuePtr> args) {
    if (proc->isType(ValueType::BUILTIN)) {
        return static_cast<BuiltinProcValue*>(proc.get())->call(args);
    } else {
        throw LispError("Unimplemented");
    }
}

ValuePtr EvalEnv::eval(ValuePtr expr) {
    if (isSelfEvaluating(expr)) {

        return expr;

    } else if (expr->isType(ValueType::NIL)) {

        throw LispError("Evaluating nil is prohibited.");

    } else if (expr->isType(ValueType::PAIR)) {

        using namespace std::literals;
        auto pairList = static_cast<PairValue*>(expr.get());
        auto exprs = pairList->toVector();

        if (auto name = pairList->getCar()->asSymbol()) {

            if (auto specialForm = SPECIAL_FORMS.find(name.value()); specialForm != SPECIAL_FORMS.end()) {

                return (specialForm->second)(pairList->getCdr()->toVector(), *this);

            } else {

                ValuePtr proc = eval(exprs[0]);
                std::vector<ValuePtr> args = evalList(pairList->getCdr());
                return this->apply(proc, args);

            }

        } else {

            throw LispError("Unimplemented.");

        }      

    } else if (expr->isType(ValueType::SYMBOL)) {

        if (auto symbol = expr->asSymbol()) {
            if (auto value = lookup(symbol.value())) {
                return value;
            } else {
                throw LispError("Variable " + symbol.value() + " not defined.");
            }
        }

    }
    
    else {
        throw LispError("Unimplemented.");
    }

    return nullptr;
}

std::vector<ValuePtr> EvalEnv::evalList(ValuePtr expr) {
    std::vector<ValuePtr> result;
    std::ranges::transform(
        expr->toVector(),
        std::back_inserter(result),
        [this](ValuePtr value) { return this->eval(value); }
    );
    return result;
}