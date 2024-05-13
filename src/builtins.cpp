#include <iostream>
#include <unordered_map>
#include <cmath>

#include "./builtins.h"
#include "./error.h"

std::unordered_map<std::string, std::shared_ptr<BuiltinProcValue> > builtins::builtins;

void builtins::init() {
    builtins::builtins.insert(builtins::core_builtins.begin(), builtins::core_builtins.end());
    builtins::builtins.insert(builtins::type_checking_builtins.begin(), builtins::type_checking_builtins.end());
    builtins::builtins.insert(builtins::cons_list_builtins.begin(), builtins::cons_list_builtins.end());
    builtins::builtins.insert(builtins::math_builtins.begin(), builtins::math_builtins.end());
    builtins::builtins.insert(builtins::comparison_builtins.begin(), builtins::comparison_builtins.end());
}


// core library

std::unordered_map<std::string, std::shared_ptr<BuiltinProcValue> > builtins::core_builtins = {

    {"display", std::make_shared<BuiltinProcValue>(&builtins::display)},
    {"exit", std::make_shared<BuiltinProcValue>(&builtins::exit)},
    {"newline", std::make_shared<BuiltinProcValue>(&builtins::newline)},
    {"print", std::make_shared<BuiltinProcValue>(&builtins::print)},

};

ValuePtr builtins::display(const std::vector<ValuePtr>& params) {
    if (params.size() < 0 || params.size() > 1) {
        throw LispError("Print requires one argument.");
    }
    if (params[0]->isType(ValueType::STRING)) {
        std::string str = static_cast<StringValue*>(params[0].get())->asString().value();
        std::cout << str << std::endl;
    } else {
        std::cout << params[0]->toString() << std::endl;
    }
    return std::make_shared<NilValue>();
}

ValuePtr builtins::exit(const std::vector<ValuePtr>& params) {
    if (params.size() > 1) {
        throw LispError("Exit does not take more than one arguments.");
    }
    // 以 n 作为进程退出码退出解释流程
    std::exit(params.size() == 0 ? 0 : params[0]->asNumber().value());
    return std::make_shared<NilValue>();
}

ValuePtr builtins::newline(const std::vector<ValuePtr>& params) {
    if (params.size() > 0) {
        throw LispError("Newline does not take any arguments.");
    }
    std::cout << std::endl;
    return std::make_shared<NilValue>();
}

ValuePtr builtins::print(const std::vector<ValuePtr>& params) {
    if (params.size() < 0 || params.size() > 1) {
        throw LispError("Print requires one argument.");
    }
    std::cout << params[0]->toString() << std::endl;
    return std::make_shared<NilValue>();
}


// type checking library

std::unordered_map<std::string, std::shared_ptr<BuiltinProcValue> > builtins::type_checking_builtins = {

    {"atom?", std::make_shared<BuiltinProcValue>(&builtins::isAtom)},
    {"boolean?", std::make_shared<BuiltinProcValue>(&builtins::isBoolean)},
    {"integer?", std::make_shared<BuiltinProcValue>(&builtins::isInteger)},
    {"list??", std::make_shared<BuiltinProcValue>(&builtins::isList)},
    {"number?", std::make_shared<BuiltinProcValue>(&builtins::isNumber)},
    {"null?", std::make_shared<BuiltinProcValue>(&builtins::isNull)},
    {"pair?", std::make_shared<BuiltinProcValue>(&builtins::isPair)},
    {"procedure?", std::make_shared<BuiltinProcValue>(&builtins::isProcedure)},
    {"string?", std::make_shared<BuiltinProcValue>(&builtins::isString)},
    {"symbol?", std::make_shared<BuiltinProcValue>(&builtins::isSymbol)},

};

// atom = bool or number or string or symbol or nil
ValuePtr builtins::isAtom(const std::vector<ValuePtr>& params) {
    if (params.size() != 1) {
        throw LispError("Atom? requires one argument.");
    }
    bool flag = params[0]->isType(ValueType::BOOLEAN) || 
                params[0]->isType(ValueType::NUMERIC) || 
                params[0]->isType(ValueType::STRING)  || 
                params[0]->isType(ValueType::SYMBOL)  || 
                params[0]->isType(ValueType::NIL);
    return std::make_shared<BooleanValue>(flag);
}

ValuePtr builtins::isBoolean(const std::vector<ValuePtr>& params) {
    if (params.size() != 1) {
        throw LispError("Boolean? requires one argument.");
    }
    return std::make_shared<BooleanValue>(params[0]->isType(ValueType::BOOLEAN));
}

ValuePtr builtins::isInteger(const std::vector<ValuePtr>& params) {
    if (params.size() != 1) {
        throw LispError("Integer? requires one argument.");
    }
    return std::make_shared<BooleanValue>(
        params[0]->isType(ValueType::NUMERIC) && 
        std::floor(params[0]->asNumber().value()) == params[0]->asNumber().value()
    );
}

ValuePtr builtins::isList(const std::vector<ValuePtr>& params) {
    if (params.size() != 1) {
        throw LispError("List? requires one argument.");
    }
    if (!params[0]->isType(ValueType::PAIR)) {
        return std::make_shared<BooleanValue>(false);
    }
    auto p = static_cast<PairValue*>(params[0].get());
    while (p->getCdr()->isType(ValueType::PAIR)) {
        p = static_cast<PairValue*>(p->getCdr().get());
    }
    return std::make_shared<BooleanValue>(p->getCdr()->isType(ValueType::NIL));
}

ValuePtr builtins::isNumber(const std::vector<ValuePtr>& params) {
    if (params.size() != 1) {
        throw LispError("Number? requires one argument.");
    }
    return std::make_shared<BooleanValue>(params[0]->isType(ValueType::NUMERIC));
}

ValuePtr builtins::isNull(const std::vector<ValuePtr>& params) {
    if (params.size() != 1) {
        throw LispError("Null? requires one argument.");
    }
    return std::make_shared<BooleanValue>(params[0]->isType(ValueType::NIL));
}

ValuePtr builtins::isPair(const std::vector<ValuePtr>& params) {
    if (params.size() != 1) {
        throw LispError("Pair? requires one argument.");
    }
    return std::make_shared<BooleanValue>(params[0]->isType(ValueType::PAIR));
}

ValuePtr builtins::isProcedure(const std::vector<ValuePtr>& params) {
    if (params.size() != 1) {
        throw LispError("Procedure? requires one argument.");
    }
    return std::make_shared<BooleanValue>(params[0]->isType(ValueType::BUILTIN));
}

ValuePtr builtins::isString(const std::vector<ValuePtr>& params) {
    if (params.size() != 1) {
        throw LispError("String? requires one argument.");
    }
    return std::make_shared<BooleanValue>(params[0]->isType(ValueType::STRING));
}

ValuePtr builtins::isSymbol(const std::vector<ValuePtr>& params) {
    if (params.size() != 1) {
        throw LispError("Symbol? requires one argument.");
    }
    return std::make_shared<BooleanValue>(params[0]->isType(ValueType::SYMBOL));
}


// cons_list library
std::unordered_map<std::string, std::shared_ptr<BuiltinProcValue> > builtins::cons_list_builtins = {

    {"car", std::make_shared<BuiltinProcValue>(&builtins::car)},
    {"cdr", std::make_shared<BuiltinProcValue>(&builtins::cdr)},
    {"cons", std::make_shared<BuiltinProcValue>(&builtins::cons)},
    {"length", std::make_shared<BuiltinProcValue>(&builtins::length)},
    {"list", std::make_shared<BuiltinProcValue>(&builtins::list)},
    {"append", std::make_shared<BuiltinProcValue>(&builtins::append)},

};

ValuePtr builtins::car(const std::vector<ValuePtr>& params) {
    if (params.size() != 1) {
        throw LispError("Car requires one argument.");
    }
    if (!params[0]->isType(ValueType::PAIR)) {
        throw LispError("Car requires a pair.");
    }
    return static_cast<PairValue*>(params[0].get())->getCar();
}

ValuePtr builtins::cdr(const std::vector<ValuePtr>& params) {
    if (params.size() != 1) {
        throw LispError("Cdr requires one argument.");
    }
    if (!params[0]->isType(ValueType::PAIR)) {
        throw LispError("Cdr requires a pair.");
    }
    return static_cast<PairValue*>(params[0].get())->getCdr();
}

ValuePtr builtins::cons(const std::vector<ValuePtr>& params) {
    if (params.size() != 2) {
        throw LispError("Cons requires two arguments.");
    }
    return std::make_shared<PairValue>(params[0], params[1]);
}

ValuePtr builtins::length(const std::vector<ValuePtr>& params) {
    if (params.size() != 1) {
        throw LispError("Length requires one argument.");
    }
    if (!params[0]->isType(ValueType::PAIR)) {
        throw LispError("Length requires a pair.");
    }
    int result = 0;
    auto p = static_cast<PairValue*>(params[0].get());
    while (p->getCdr()->isType(ValueType::PAIR)) {
        p = static_cast<PairValue*>(p->getCdr().get());
        result++;
    }
    result++;
    return std::make_shared<NumericValue>(result);
}

ValuePtr builtins::list(const std::vector<ValuePtr>& params) {
    return std::make_shared<PairValue>(params);
}

ValuePtr builtins::append(const std::vector<ValuePtr>& params) {
    if (params.size() < 1) {
        throw LispError("Append requires at least one argument.");
    }
    std::vector<ValuePtr> result;
    for (const auto& i : params) {
        if (!i->isType(ValueType::PAIR)) {
            throw LispError("Append requires a pair.");
        }
        auto p = static_cast<PairValue*>(i.get());
        while (p->getCdr()->isType(ValueType::PAIR)) {
            result.push_back(p->getCar());
            p = static_cast<PairValue*>(p->getCdr().get());
        }
        result.push_back(p->getCar());
    }
    return std::make_shared<PairValue>(result);
}

// math library

std::unordered_map<std::string, std::shared_ptr<BuiltinProcValue> > builtins::math_builtins = {

    {"+", std::make_shared<BuiltinProcValue>(&builtins::addVal)},
    {"-", std::make_shared<BuiltinProcValue>(&builtins::subVal)},
    {"*", std::make_shared<BuiltinProcValue>(&builtins::mulVal)},
    {"/", std::make_shared<BuiltinProcValue>(&builtins::divVal)},
    {"abs", std::make_shared<BuiltinProcValue>(&builtins::absVal)},
    {"expt", std::make_shared<BuiltinProcValue>(&builtins::exptVal)},
    {"quotient", std::make_shared<BuiltinProcValue>(&builtins::quotientVal)},
    {"remainder", std::make_shared<BuiltinProcValue>(&builtins::remainderVal)},
    {"modulo", std::make_shared<BuiltinProcValue>(&builtins::moduloVal)},

};

ValuePtr builtins::addVal(const std::vector<ValuePtr>& params) {
    double result = 0;
    for (const auto& i : params) {
        if (!i->isType(ValueType::NUMERIC)) {
            throw LispError("Cannot add a non-numeric value.");
        }
        result += i->asNumber().value();
    }
    return std::make_shared<NumericValue>(result);
}

ValuePtr builtins::subVal(const std::vector<ValuePtr>& params) {
    if (params.size() < 1 || params.size() > 2) {
        throw LispError("Subtraction requires two argument.");
    } 
    if (!params[0]->isType(ValueType::NUMERIC) || !params[1]->isType(ValueType::NUMERIC)) {
        throw LispError("Cannot subtract a non-numeric value.");
    }
    if (params.size() == 1) {
        return std::make_shared<NumericValue>(-params[0]->asNumber().value());
    }
    double result = params[0]->asNumber().value() - params[1]->asNumber().value();

    return std::make_shared<NumericValue>(result);
}

ValuePtr builtins::mulVal(const std::vector<ValuePtr>& params) {
    double result = 1;
    for (const auto& i : params) {
        if (!i->isType(ValueType::NUMERIC)) {
            throw LispError("Cannot multiply a non-numeric value.");
        }
        result *= i->asNumber().value();
    }
    return std::make_shared<NumericValue>(result);
}

ValuePtr builtins::divVal(const std::vector<ValuePtr>& params) {
    if (params.size() < 1 || params.size() > 2) {
        throw LispError("Division requires two arguments.");
    }
    if (!params[0]->isType(ValueType::NUMERIC) || !params[1]->isType(ValueType::NUMERIC)) {
        throw LispError("Cannot divide a non-numeric value.");
    }
    if (params.size() == 1) {
        if (params[0]->asNumber().value() == 0) {
            throw LispError("Cannot divide by zero.");
        }
        return std::make_shared<NumericValue>(1 / params[0]->asNumber().value());
    }
    if (params[1]->asNumber().value() == 0) {
        throw LispError("Cannot divide by zero.");
    }
    double result = params[0]->asNumber().value() / params[1]->asNumber().value();
    
    return std::make_shared<NumericValue>(result);
}

ValuePtr builtins::absVal(const std::vector<ValuePtr>& params) {
    if (params.size() != 1) {
        throw LispError("Abs requires one argument.");
    }
    if (!params[0]->isType(ValueType::NUMERIC)) {
        throw LispError("Cannot get the absolute value of a non-numeric value.");
    }
    return std::make_shared<NumericValue>(std::abs(params[0]->asNumber().value()));
}

ValuePtr builtins::exptVal(const std::vector<ValuePtr>& params) {
    if (params.size() != 2) {
        throw LispError("Expt requires two arguments.");
    }
    if (!params[0]->isType(ValueType::NUMERIC) || !params[1]->isType(ValueType::NUMERIC)) {
        throw LispError("Cannot raise a non-numeric value to a power.");
    }
    if (params[0]->asNumber().value() == 0 && params[1]->asNumber().value() <= 0) {
        throw LispError("Cannot raise zero to a non-positive power.");
    }
    return std::make_shared<NumericValue>(std::pow(params[0]->asNumber().value(), params[1]->asNumber().value()));
}

ValuePtr builtins::quotientVal(const std::vector<ValuePtr>& params) {
    if (params.size() != 2) {
        throw LispError("Quotient requires two arguments.");
    }
    if (!params[0]->isType(ValueType::NUMERIC) || !params[1]->isType(ValueType::NUMERIC)) {
        throw LispError("Cannot divide a non-numeric value.");
    }
    return std::make_shared<NumericValue>(static_cast<int>(params[0]->asNumber().value() / params[1]->asNumber().value()));
}

ValuePtr builtins::remainderVal(const std::vector<ValuePtr>& params) {
    if (params.size() != 2) {
        throw LispError("Remainder requires two arguments.");
    }
    if (!params[0]->isType(ValueType::NUMERIC) || !params[1]->isType(ValueType::NUMERIC)) {
        throw LispError("Cannot divide a non-numeric value.");
    }
    if (params[1]->asNumber().value() == 0) {
        throw LispError("Cannot divide by zero.");
    }
    return std::make_shared<NumericValue>(static_cast<int>(params[0]->asNumber().value()) % std::abs(static_cast<int>(params[1]->asNumber().value())));
}

ValuePtr builtins::moduloVal(const std::vector<ValuePtr>& params) {
    if (params.size() != 2) {
        throw LispError("Modulo requires two arguments.");
    }
    if (!params[0]->isType(ValueType::NUMERIC) || !params[1]->isType(ValueType::NUMERIC)) {
        throw LispError("Cannot perform modulo on non-numeric values.");
    }
    double x = params[0]->asNumber().value();
    double y = params[1]->asNumber().value();
    if (y == 0) {
        throw LispError("Cannot divide by zero.");
    }
    int result = static_cast<int>(std::abs(x)) % static_cast<int>(std::abs(y));
    if (x * y < 0) {
        result = std::abs(y) - result;
    }
    return std::make_shared<NumericValue>(y > 0 ? result : -result);
}

// comparison library

std::unordered_map<std::string, std::shared_ptr<BuiltinProcValue> > builtins::comparison_builtins = {

    {"=", std::make_shared<BuiltinProcValue>(&builtins::eqVal)},
    {"<", std::make_shared<BuiltinProcValue>(&builtins::ltVal)},
    {">", std::make_shared<BuiltinProcValue>(&builtins::gtVal)},
    {"<=", std::make_shared<BuiltinProcValue>(&builtins::leVal)},
    {">=", std::make_shared<BuiltinProcValue>(&builtins::geVal)},
    {"even?", std::make_shared<BuiltinProcValue>(&builtins::isEven)},
    {"odd?", std::make_shared<BuiltinProcValue>(&builtins::isOdd)},
    {"zero?", std::make_shared<BuiltinProcValue>(&builtins::isZero)},

};

ValuePtr builtins::eqVal(const std::vector<ValuePtr>& params) {
    if (params.size() != 2) {
        throw LispError("= requires two arguments.");
    }
    if (!params[0]->isType(ValueType::NUMERIC) || !params[1]->isType(ValueType::NUMERIC)) {
        throw LispError("Cannot compare a non-numeric value.");
    }
    return std::make_shared<BooleanValue>(params[0]->asNumber().value() == params[1]->asNumber().value());
}

ValuePtr builtins::ltVal(const std::vector<ValuePtr>& params) {
    if (params.size() != 2) {
        throw LispError("< requires two arguments.");
    }
    if (!params[0]->isType(ValueType::NUMERIC) || !params[1]->isType(ValueType::NUMERIC)) {
        throw LispError("Cannot compare a non-numeric value.");
    }
    return std::make_shared<BooleanValue>(params[0]->asNumber().value() < params[1]->asNumber().value());
}

ValuePtr builtins::gtVal(const std::vector<ValuePtr>& params) {
    if (params.size() != 2) {
        throw LispError("> requires two arguments.");
    }
    if (!params[0]->isType(ValueType::NUMERIC) || !params[1]->isType(ValueType::NUMERIC)) {
        throw LispError("Cannot compare a non-numeric value.");
    }
    return std::make_shared<BooleanValue>(params[0]->asNumber().value() > params[1]->asNumber().value());
}

ValuePtr builtins::leVal(const std::vector<ValuePtr>& params) {
    if (params.size() != 2) {
        throw LispError("<= requires two arguments.");
    }
    if (!params[0]->isType(ValueType::NUMERIC) || !params[1]->isType(ValueType::NUMERIC)) {
        throw LispError("Cannot compare a non-numeric value.");
    }
    return std::make_shared<BooleanValue>(params[0]->asNumber().value() <= params[1]->asNumber().value());
}

ValuePtr builtins::geVal(const std::vector<ValuePtr>& params) {
    if (params.size() != 2) {
        throw LispError(">= requires two arguments.");
    }
    if (!params[0]->isType(ValueType::NUMERIC) || !params[1]->isType(ValueType::NUMERIC)) {
        throw LispError("Cannot compare a non-numeric value.");
    }
    return std::make_shared<BooleanValue>(params[0]->asNumber().value() >= params[1]->asNumber().value());
}

ValuePtr builtins::isEven(const std::vector<ValuePtr>& params) {
    if (params.size() != 1) {
        throw LispError("Even? requires one argument.");
    }
    if (!params[0]->isType(ValueType::NUMERIC)) {
        throw LispError("Cannot compare a non-numeric value.");
    }
    return std::make_shared<BooleanValue>(static_cast<int>(params[0]->asNumber().value()) % 2 == 0);
}

ValuePtr builtins::isOdd(const std::vector<ValuePtr>& params) {
    if (params.size() != 1) {
        throw LispError("Odd? requires one argument.");
    }
    if (!params[0]->isType(ValueType::NUMERIC)) {
        throw LispError("Cannot compare a non-numeric value.");
    }
    return std::make_shared<BooleanValue>(static_cast<int>(params[0]->asNumber().value()) % 2 == 1);
}

ValuePtr builtins::isZero(const std::vector<ValuePtr>& params) {
    if (params.size() != 1) {
        throw LispError("Zero? requires one argument.");
    }
    if (!params[0]->isType(ValueType::NUMERIC)) {
        throw LispError("Cannot compare a non-numeric value.");
    }
    return std::make_shared<BooleanValue>(params[0]->asNumber().value() == 0);
}