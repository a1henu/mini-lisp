#include <algorithm>
#include <ranges>
#include <iostream>
#include <unordered_map>
#include <cmath>

#include "./builtins.h"
#include "./error.h"
#include "./eval_env.h"
#include "./parser.h"
#include "./tokenizer.h"

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

    {"apply", std::make_shared<BuiltinProcValue>(&builtins::apply)},
    {"display", std::make_shared<BuiltinProcValue>(&builtins::display)},
    {"displayln", std::make_shared<BuiltinProcValue>(&builtins::displayln)},
    {"error", std::make_shared<BuiltinProcValue>(&builtins::error)},
    {"eval", std::make_shared<BuiltinProcValue>(&builtins::eval)},
    {"exit", std::make_shared<BuiltinProcValue>(&builtins::exit)},
    {"newline", std::make_shared<BuiltinProcValue>(&builtins::newline)},
    {"print", std::make_shared<BuiltinProcValue>(&builtins::print)},
    {"readline", std::make_shared<BuiltinProcValue>(&builtins::readline)},
    {"help", std::make_shared<BuiltinProcValue>(&builtins::help)},

};

ValuePtr builtins::apply(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() != 2) {
        throw LispError("Apply requires at least two arguments.");
    }

    if (!params[0]->isType(ValueType::BUILTIN) &&
        !params[0]->isType(ValueType::LAMBDA)
    ){
        throw LispError("Apply requires a procedure as its first argument.");
    }
    if (!params[1]->isType(ValueType::PAIR)) {
        throw LispError("Apply requires a list as its second argument.");
    }
    auto proc = params[0];
    std::vector<ValuePtr> args;
    std::ranges::transform(
        params[1]->toVector(),
        std::back_inserter(args),
        [&env](ValuePtr v) { 
            if (v->isType(ValueType::PAIR)) {
                return v;
            } else {
                return env.eval(v);
            }
        }
    );
    return proc->call(args, env);
} 

ValuePtr builtins::display(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() < 1) {
        throw LispError("Print requires more than one argument.");
    }
    for (const auto& i : params) {
        if (i->isType(ValueType::STRING)) {
            std::string str = static_cast<StringValue*>(i.get())->asString().value();
            std::cout << str;
        } else {
            std::cout << i->toString();
        }
    }
    return std::make_shared<NilValue>();
}

ValuePtr builtins::displayln(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() < 0 || params.size() > 1) {
        throw LispError("Print requires one argument.");
    }
    if (params[0]->isType(ValueType::STRING)) {
        std::string str = static_cast<StringValue*>(params[0].get())->asString().value();
        std::cout << str;
    } else {
        std::cout << params[0]->toString();
    }
    std::cout << std::endl;
    return std::make_shared<NilValue>();
}

ValuePtr builtins::error(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() != 1) {
        throw LispError("Error requires one argument.");
    }
    throw LispError(params[0]->toString());
}

ValuePtr builtins::eval(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() != 1) {
        throw LispError("Eval requires one argument.");
    }
    return env.eval(params[0]);
}

ValuePtr builtins::exit(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() > 1) {
        throw LispError("Exit does not take more than one arguments.");
    }
    // Exit the interpreter process with n as the exit code
    std::exit(params.size() == 0 ? 0 : params[0]->asNumber().value());
    return std::make_shared<NilValue>();
}

ValuePtr builtins::newline(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() > 0) {
        throw LispError("Newline does not take any arguments.");
    }
    std::cout << std::endl;
    return std::make_shared<NilValue>();
}

ValuePtr builtins::print(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() < 1) {
        throw LispError("Print requires more than one argument.");
    }
    for (const auto& i : params) {
        std::cout << i->toString() << std::endl;
    }
    return std::make_shared<NilValue>();
}

ValuePtr builtins::readline(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() > 0) {
        throw LispError("Read does not take any arguments.");
    }
    std::cout << "> ";
    std::string line;
    std::getline(std::cin, line);
    auto tokens = Tokenizer::tokenize(line);
    Parser parser(std::move(tokens));
    auto value = parser.parse();
    auto result = env.eval(std::move(value));
    return result;
}

ValuePtr builtins::help(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() > 0) {
        throw LispError("Help does not take any arguments.");
    }
    std::cout << "Welcome to Mini-Lisp Help Utility" << std::endl;
    std::cout << "Please refer to the documentation for more information." << std::endl;
    std::cout << "https://pku-software.github.io/mini-lisp-spec/" << std::endl;
    std::cout << "The following built-in functions are available:" << std::endl;
    std::cout << "Core functions:" << std::endl;
    for (const auto& i : builtins::core_builtins) {
        std::cout << i.first << std::endl;
    }
    std::cout << std::endl << "- Type checking functions:" << std::endl;
    for (const auto& i : builtins::type_checking_builtins) {
        std::cout << i.first << std::endl;
    }
    std::cout << std::endl << "- Cons list functions:" << std::endl;
    for (const auto& i : builtins::cons_list_builtins) {
        std::cout << i.first << std::endl;
    }
    std::cout << std::endl << "- Math functions:" << std::endl;
    for (const auto& i : builtins::math_builtins) {
        std::cout << i.first << std::endl;
    }
    std::cout << std::endl << "- Comparison functions:" << std::endl;
    for (const auto& i : builtins::comparison_builtins) {
        std::cout << i.first << std::endl;
    }
    return std::make_shared<NilValue>();
}


// type checking library

std::unordered_map<std::string, std::shared_ptr<BuiltinProcValue> > builtins::type_checking_builtins = {

    {"atom?", std::make_shared<BuiltinProcValue>(&builtins::isAtom)},
    {"boolean?", std::make_shared<BuiltinProcValue>(&builtins::isBoolean)},
    {"integer?", std::make_shared<BuiltinProcValue>(&builtins::isInteger)},
    {"list?", std::make_shared<BuiltinProcValue>(&builtins::isList)},
    {"number?", std::make_shared<BuiltinProcValue>(&builtins::isNumber)},
    {"null?", std::make_shared<BuiltinProcValue>(&builtins::isNull)},
    {"pair?", std::make_shared<BuiltinProcValue>(&builtins::isPair)},
    {"procedure?", std::make_shared<BuiltinProcValue>(&builtins::isProcedure)},
    {"string?", std::make_shared<BuiltinProcValue>(&builtins::isString)},
    {"symbol?", std::make_shared<BuiltinProcValue>(&builtins::isSymbol)},

};

// atom = bool or number or string or symbol or nil
ValuePtr builtins::isAtom(const std::vector<ValuePtr>& params, EvalEnv& env) {
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

ValuePtr builtins::isBoolean(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() != 1) {
        throw LispError("Boolean? requires one argument.");
    }
    return std::make_shared<BooleanValue>(params[0]->isType(ValueType::BOOLEAN));
}

ValuePtr builtins::isInteger(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() != 1) {
        throw LispError("Integer? requires one argument.");
    }
    return std::make_shared<BooleanValue>(
        params[0]->isType(ValueType::NUMERIC) && 
        std::floor(params[0]->asNumber().value()) == params[0]->asNumber().value()
    );
}

ValuePtr builtins::isList(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() != 1) {
        throw LispError("List? requires one argument.");
    }
    if (!params[0]->isType(ValueType::PAIR) && !params[0]->isType(ValueType::NIL)) {
        return std::make_shared<BooleanValue>(false);
    }
    if (params[0]->isType(ValueType::NIL)) {
        return std::make_shared<BooleanValue>(true);
    }
    auto p = static_cast<PairValue*>(params[0].get());
    while (p->getCdr()->isType(ValueType::PAIR)) {
        p = static_cast<PairValue*>(p->getCdr().get());
    }
    return std::make_shared<BooleanValue>(p->getCdr()->isType(ValueType::NIL));
}

ValuePtr builtins::isNumber(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() != 1) {
        throw LispError("Number? requires one argument.");
    }
    return std::make_shared<BooleanValue>(params[0]->isType(ValueType::NUMERIC));
}

ValuePtr builtins::isNull(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() != 1) {
        throw LispError("Null? requires one argument.");
    }
    return std::make_shared<BooleanValue>(params[0]->isType(ValueType::NIL));
}

ValuePtr builtins::isPair(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() != 1) {
        throw LispError("Pair? requires one argument.");
    }
    return std::make_shared<BooleanValue>(params[0]->isType(ValueType::PAIR));
}

ValuePtr builtins::isProcedure(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() != 1) {
        throw LispError("Procedure? requires one argument.");
    }
    return std::make_shared<BooleanValue>(
        params[0]->isType(ValueType::BUILTIN) ||
        params[0]->isType(ValueType::LAMBDA)
    );
}

ValuePtr builtins::isString(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() != 1) {
        throw LispError("String? requires one argument.");
    }
    return std::make_shared<BooleanValue>(params[0]->isType(ValueType::STRING));
}

ValuePtr builtins::isSymbol(const std::vector<ValuePtr>& params, EvalEnv& env) {
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
    {"map", std::make_shared<BuiltinProcValue>(&builtins::map)},
    {"filter", std::make_shared<BuiltinProcValue>(&builtins::filter)},
    {"reduce", std::make_shared<BuiltinProcValue>(&builtins::reduce)},

};

ValuePtr builtins::car(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() != 1) {
        throw LispError("Car requires one argument.");
    }
    if (!params[0]->isType(ValueType::PAIR)) {
        throw LispError("Car requires a pair.");
    }
    return static_cast<PairValue*>(params[0].get())->getCar();
}

ValuePtr builtins::cdr(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() != 1) {
        throw LispError("Cdr requires one argument.");
    }
    if (!params[0]->isType(ValueType::PAIR)) {
        throw LispError("Cdr requires a pair.");
    }
    return static_cast<PairValue*>(params[0].get())->getCdr();
}

ValuePtr builtins::cons(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() != 2) {
        throw LispError("Cons requires two arguments.");
    }
    return std::make_shared<PairValue>(params[0], params[1]);
}

ValuePtr builtins::length(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() != 1) {
        throw LispError("Length requires one argument.");
    }
    if (!params[0]->isType(ValueType::PAIR) && !params[0]->isType(ValueType::NIL)) {
        throw LispError("Length requires a pair.");
    }
    if (params[0]->isType(ValueType::NIL)) {
        return std::make_shared<NumericValue>(0);
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

ValuePtr builtins::list(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() == 0) {
        return std::make_shared<NilValue>();
    }
    return std::make_shared<PairValue>(params);
}

ValuePtr builtins::append(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() == 0) {
        return std::make_shared<NilValue>();
    }
    std::vector<ValuePtr> result;
    for (const auto& i : params) {
        if (i->isType(ValueType::PAIR)) {
            auto list = i->toVector();
            result.insert(result.end(), list.begin(), list.end());
        } else if (!i->isType(ValueType::NIL)) {
            throw LispError("Append requires lists or nil.");
        }
    }
    if (result.empty()) {
        return std::make_shared<NilValue>();
    } else {
        return std::make_shared<PairValue>(result);
    }
}

ValuePtr builtins::map(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() != 2) {
        throw LispError("Map requires two arguments.");
    }
    if (!params[0]->isType(ValueType::BUILTIN) && !params[0]->isType(ValueType::LAMBDA)) {
        throw LispError("Map requires a procedure as its first argument.");
    }
    if (!params[1]->isType(ValueType::PAIR)) {
        throw LispError("Map requires a pair as its second argument.");
    }
    auto proc = params[0];
    auto list = params[1]->toVector();
    std::vector<ValuePtr> result;
    std::ranges::transform(
        list, 
        std::back_inserter(result), 
        [proc, &env](ValuePtr v) { return proc->call({v}, env); }
    );
    return std::make_shared<PairValue>(result);
}

ValuePtr builtins::filter(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() != 2) {
        throw LispError("Filter requires two arguments.");
    }
    if (!params[0]->isType(ValueType::BUILTIN) && !params[0]->isType(ValueType::LAMBDA)) {
        throw LispError("Filter requires a procedure as its first argument.");
    }
    if (!params[1]->isType(ValueType::PAIR)) {
        if (params[1]->isType(ValueType::NIL)) {
            return std::make_shared<NilValue>();
        } else {
            throw LispError("Filter requires a pair as its second argument.");
        }
    }
    auto proc = params[0];
    auto list = params[1]->toVector();
    std::vector<ValuePtr> result;
    std::ranges::copy_if(
        list, 
        std::back_inserter(result), 
        [proc, &env](ValuePtr v) { return proc->call({v}, env)->asBoolean(); }
    );
    if (result.empty()) {
        return std::make_shared<NilValue>();
    } else {
        return std::make_shared<PairValue>(result);
    }
}

ValuePtr reduce_recursive(ValuePtr proc, std::vector<ValuePtr> list, EvalEnv& env) {
    if (list.size() == 0) {
        throw LispError("Cannot reduce an empty list.");
    }
    if (list.size() == 1) {
        return list[0];
    }
    return proc->call(
        std::vector<ValuePtr>({
            list[0], 
            reduce_recursive(proc, std::vector(list.begin() + 1, list.end()), env)
        }), 
        env
    );
}

ValuePtr builtins::reduce(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() != 2) {
        throw LispError("Reduce requires three arguments.");
    }
    if (!params[0]->isType(ValueType::BUILTIN) && !params[0]->isType(ValueType::LAMBDA)) {
        throw LispError("Reduce requires a procedure as its first argument.");
    }
    if (!params[1]->isType(ValueType::PAIR)) {
        throw LispError("Reduce requires a pair as its second argument.");
    }
    auto proc = params[0];
    auto list = params[1]->toVector();
    return reduce_recursive(proc, list, env);
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

ValuePtr builtins::addVal(const std::vector<ValuePtr>& params, EvalEnv& env) {
    double result = 0;
    for (const auto& i : params) {
        if (!i->isType(ValueType::NUMERIC)) {
            throw LispError("Cannot add a non-numeric value.");
        }
        result += i->asNumber().value();
    }
    return std::make_shared<NumericValue>(result);
}

ValuePtr builtins::subVal(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() < 1 || params.size() > 2) {
        throw LispError("Subtraction requires two argument.");
    } 
    double result;
    if (params.size() == 1) {
        if (!params[0]->isType(ValueType::NUMERIC)) {
            throw LispError("Cannot subtract a non-numeric value.");
        } else {
            result = -params[0]->asNumber().value();
        }
    } else {
        if (!params[0]->isType(ValueType::NUMERIC) || !params[1]->isType(ValueType::NUMERIC)) {
            throw LispError("Cannot subtract a non-numeric value.");
        } else {
            result = params[0]->asNumber().value() - params[1]->asNumber().value();
        }
    }
    return std::make_shared<NumericValue>(result);
}

ValuePtr builtins::mulVal(const std::vector<ValuePtr>& params, EvalEnv& env) {
    double result = 1;
    for (const auto& i : params) {
        if (!i->isType(ValueType::NUMERIC)) {
            throw LispError("Cannot multiply a non-numeric value.");
        }
        result *= i->asNumber().value();
    }
    return std::make_shared<NumericValue>(result);
}

ValuePtr builtins::divVal(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() < 1 || params.size() > 2) {
        throw LispError("Division requires two arguments.");
    }
    double result;
    if (params.size() == 1) {
        if (params[0]->asNumber().value() == 0) {
            throw LispError("Cannot divide by zero.");
        }
        if (!params[0]->isType(ValueType::NUMERIC)) {
            throw LispError("Cannot divide a non-numeric value.");
        }
        result = 1 / params[0]->asNumber().value();
    } else {
        if (params[1]->asNumber().value() == 0) {
            throw LispError("Cannot divide by zero.");
        }
        if (!params[0]->isType(ValueType::NUMERIC) || !params[1]->isType(ValueType::NUMERIC)) {
            throw LispError("Cannot divide a non-numeric value.");
        }
        result = params[0]->asNumber().value() / params[1]->asNumber().value();
    }
    return std::make_shared<NumericValue>(result);
}

ValuePtr builtins::absVal(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() != 1) {
        throw LispError("Abs requires one argument.");
    }
    if (!params[0]->isType(ValueType::NUMERIC)) {
        throw LispError("Cannot get the absolute value of a non-numeric value.");
    }
    return std::make_shared<NumericValue>(std::abs(params[0]->asNumber().value()));
}

ValuePtr builtins::exptVal(const std::vector<ValuePtr>& params, EvalEnv& env) {
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

ValuePtr builtins::quotientVal(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() != 2) {
        throw LispError("Quotient requires two arguments.");
    }
    if (!params[0]->isType(ValueType::NUMERIC) || !params[1]->isType(ValueType::NUMERIC)) {
        throw LispError("Cannot divide a non-numeric value.");
    }
    return std::make_shared<NumericValue>(static_cast<int>(params[0]->asNumber().value() / params[1]->asNumber().value()));
}

ValuePtr builtins::remainderVal(const std::vector<ValuePtr>& params, EvalEnv& env) {
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

ValuePtr builtins::moduloVal(const std::vector<ValuePtr>& params, EvalEnv& env) {
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

    {"eq?", std::make_shared<BuiltinProcValue>(&builtins::dataEq)},
    {"equal?", std::make_shared<BuiltinProcValue>(&builtins::dataEqual)},
    {"not", std::make_shared<BuiltinProcValue>(&builtins::notVal)},
    {"=", std::make_shared<BuiltinProcValue>(&builtins::eqVal)},
    {"<", std::make_shared<BuiltinProcValue>(&builtins::ltVal)},
    {">", std::make_shared<BuiltinProcValue>(&builtins::gtVal)},
    {"<=", std::make_shared<BuiltinProcValue>(&builtins::leVal)},
    {">=", std::make_shared<BuiltinProcValue>(&builtins::geVal)},
    {"even?", std::make_shared<BuiltinProcValue>(&builtins::isEven)},
    {"odd?", std::make_shared<BuiltinProcValue>(&builtins::isOdd)},
    {"zero?", std::make_shared<BuiltinProcValue>(&builtins::isZero)},

};

ValuePtr builtins::dataEq(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() != 2) {
        throw LispError("Eq? requires two arguments.");
    }
    if (params[0]->getType() != params[1]->getType()) {
        return std::make_shared<BooleanValue>(false);
    }
    if (params[0]->isType(ValueType::BOOLEAN) ||
        params[0]->isType(ValueType::NUMERIC) ||
        params[0]->isType(ValueType::BUILTIN) ||
        params[0]->isType(ValueType::SYMBOL)  ||
        params[0]->isType(ValueType::NIL)         
    ) {
        return builtins::dataEqual(params, env);
    } else if (params[0]->isType(ValueType::STRING) ||
               params[0]->isType(ValueType::PAIR)
    ) {
        return std::make_shared<BooleanValue>(params[0].get() == params[1].get());
    } else {
        throw LispError("Cannot compare this type of value.");
    }
}

ValuePtr builtins::dataEqual(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() != 2) {
        throw LispError("Eq? requires two arguments.");
    }
    if (params[0]->getType() != params[1]->getType()) {
        return std::make_shared<BooleanValue>(false);
    } else if (params[0]->isType(ValueType::BUILTIN) ||
               params[0]->isType(ValueType::LAMBDA)) {
        throw LispError("Cannot compare procedures.");
    } else if (params[0]->isType(ValueType::PAIR)) {
        auto head = static_cast<PairValue*>(params[0].get());
        auto tail = static_cast<PairValue*>(params[1].get());
        return std::make_shared<BooleanValue>(
            builtins::dataEqual(std::vector<ValuePtr>({head->getCar(), tail->getCar()}), env)->asBoolean() &&
            builtins::dataEqual(std::vector<ValuePtr>({head->getCdr(), tail->getCdr()}), env)->asBoolean()
        );
    } else if (params[0]->isType(ValueType::SYMBOL)) {
        return std::make_shared<BooleanValue>(params[0]->asSymbol().value() == params[1]->asSymbol().value());
    } else if (params[0]->isType(ValueType::STRING)) {
        return std::make_shared<BooleanValue>(params[0]->asString().value() == params[1]->asString().value());
    } else if (params[0]->isType(ValueType::NUMERIC)) {
        return std::make_shared<BooleanValue>(params[0]->asNumber().value() == params[1]->asNumber().value());
    } else if (params[0]->isType(ValueType::BOOLEAN)) {
        return std::make_shared<BooleanValue>(params[0]->asBoolean() == params[1]->asBoolean());
    } else if (params[0]->isType(ValueType::NIL)) {
        return std::make_shared<BooleanValue>(true);
    } else {
        throw LispError("Cannot compare this type of value.");
    }
}

ValuePtr builtins::notVal(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() != 1) {
        throw LispError("Not requires one argument.");
    }
    bool flag = false;
    if (params[0]->asBoolean() == false) {
        flag = true;
    }
    return std::make_shared<BooleanValue>(flag);
}

ValuePtr builtins::eqVal(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() != 2) {
        throw LispError("= requires two arguments.");
    }
    if (!params[0]->isType(ValueType::NUMERIC) || !params[1]->isType(ValueType::NUMERIC)) {
        throw LispError("Cannot compare a non-numeric value.");
    }
    return std::make_shared<BooleanValue>(params[0]->asNumber().value() == params[1]->asNumber().value());
}

ValuePtr builtins::ltVal(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() != 2) {
        throw LispError("< requires two arguments.");
    }
    if (!params[0]->isType(ValueType::NUMERIC) || !params[1]->isType(ValueType::NUMERIC)) {
        throw LispError("Cannot compare a non-numeric value.");
    }
    return std::make_shared<BooleanValue>(params[0]->asNumber().value() < params[1]->asNumber().value());
}

ValuePtr builtins::gtVal(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() != 2) {
        throw LispError("> requires two arguments.");
    }
    if (!params[0]->isType(ValueType::NUMERIC) || !params[1]->isType(ValueType::NUMERIC)) {
        throw LispError("Cannot compare a non-numeric value.");
    }
    return std::make_shared<BooleanValue>(params[0]->asNumber().value() > params[1]->asNumber().value());
}

ValuePtr builtins::leVal(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() != 2) {
        throw LispError("<= requires two arguments.");
    }
    if (!params[0]->isType(ValueType::NUMERIC) || !params[1]->isType(ValueType::NUMERIC)) {
        throw LispError("Cannot compare a non-numeric value.");
    }
    return std::make_shared<BooleanValue>(params[0]->asNumber().value() <= params[1]->asNumber().value());
}

ValuePtr builtins::geVal(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() != 2) {
        throw LispError(">= requires two arguments.");
    }
    if (!params[0]->isType(ValueType::NUMERIC) || !params[1]->isType(ValueType::NUMERIC)) {
        throw LispError("Cannot compare a non-numeric value.");
    }
    return std::make_shared<BooleanValue>(params[0]->asNumber().value() >= params[1]->asNumber().value());
}

ValuePtr builtins::isEven(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() != 1) {
        throw LispError("Even? requires one argument.");
    }
    if (!params[0]->isType(ValueType::NUMERIC)) {
        throw LispError("Cannot compare a non-numeric value.");
    }
    if (std::floor(params[0]->asNumber().value()) != params[0]->asNumber().value()) {
        throw LispError("Cannot compare a non-integer value.");
    }
    return std::make_shared<BooleanValue>(static_cast<int>(params[0]->asNumber().value()) % 2 == 0);
}

ValuePtr builtins::isOdd(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() != 1) {
        throw LispError("Odd? requires one argument.");
    }
    if (!params[0]->isType(ValueType::NUMERIC)) {
        throw LispError("Cannot compare a non-numeric value.");
    }
    if (std::floor(params[0]->asNumber().value()) != params[0]->asNumber().value()) {
        throw LispError("Cannot compare a non-integer value.");
    }
    return std::make_shared<BooleanValue>(static_cast<int>(std::abs(params[0]->asNumber().value())) % 2 == 1);
}

ValuePtr builtins::isZero(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() != 1) {
        throw LispError("Zero? requires one argument.");
    }
    if (!params[0]->isType(ValueType::NUMERIC)) {
        throw LispError("Cannot compare a non-numeric value.");
    }
    return std::make_shared<BooleanValue>(params[0]->asNumber().value() == 0);
}
