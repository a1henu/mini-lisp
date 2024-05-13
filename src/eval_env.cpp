#include "./builtins.h"
#include "./error.h"
#include "./eval_env.h"
#include "./forms.h"

EvalEnv::EvalEnv() : parent{nullptr} {
    builtins::init();
    for (const auto& [name, value] : builtins::builtins) {
        define(name, value);
    }
}
EvalEnv::EvalEnv(std::shared_ptr<EvalEnv> parent = std::make_shared<EvalEnv>(nullptr)) : parent{parent} {
    builtins::init();
    for (const auto& [name, value] : builtins::builtins) {
        define(name, value);
    }
}

void EvalEnv::define(const std::string& symbol, ValuePtr value) {
    SYMBOL_TABLE[symbol] = value;
}

ValuePtr EvalEnv::lookup(const std::string& symbol) {
    if (auto value = SYMBOL_TABLE.find(symbol); value != SYMBOL_TABLE.end()) {
        return value->second;
    } else if (parent != nullptr) {
        return parent->lookup(symbol);
    } else {
        throw LispError("Unfound symbol: " + symbol);
        return (ValuePtr)nullptr;
    }
}

bool isSelfEvaluating(ValuePtr expr) {
    return expr->isType(ValueType::BOOLEAN) || 
           expr->isType(ValueType::NUMERIC) || 
           expr->isType(ValueType::STRING);
}

std::shared_ptr<EvalEnv> EvalEnv::createChild(const std::vector<std::string>& params, const std::vector<ValuePtr>& args) {
    if (params.size() != args.size()) {
        throw LispError("Parameter size and argument size do not match.");
    }
    auto child = std::make_shared<EvalEnv>(shared_from_this());
    for (size_t i = 0; i < params.size(); i++) {
        child->define(params[i], args[i]);
    }
    return child;
}

ValuePtr EvalEnv::apply(ValuePtr proc, std::vector<ValuePtr> args) {
    if (proc->isType(ValueType::BUILTIN)) {
        return static_cast<BuiltinProcValue*>(proc.get())->call(args);
    } if (proc->isType(ValueType::LAMBDA)) {
        return static_cast<LambdaValue*>(proc.get())->call(args, *this);
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

        auto pairList = static_cast<PairValue*>(expr.get());
        auto exprs = pairList->toVector();

        if (auto name = pairList->getCar()->asSymbol()) {

            if (auto specialForm = SPECIAL_FORMS.find(*name); specialForm != SPECIAL_FORMS.end()) {

                return (specialForm->second)(pairList->getCdr()->toVector(), *this);

            } else if (auto value = lookup(name.value()); value != nullptr) {

                ValuePtr proc = eval(exprs[0]);
                std::vector<ValuePtr> args = evalList(pairList->getCdr());
                return this->apply(proc, args);

            } else {

                throw LispError("Unfound symbol: " + name.value());

            }

        } else if (pairList->getCar()->isType(ValueType::PAIR)) {
        
            ValuePtr proc = eval(pairList->getCar());
            std::vector<ValuePtr> args = evalList(pairList->getCdr());
            return this->apply(proc, args);

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