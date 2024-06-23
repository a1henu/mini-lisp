#include <algorithm>
#include <ranges>

#include "./error.h"
#include "./eval_env.h"
#include "./forms.h"

std::unordered_map<std::string, SpecialFormType*> SPECIAL_FORMS = {

    {"define", defineForm},
    {"quote", quoteForm},
    {"quasiquote", quasiquoteForm},
    {"if", ifForm},
    {"and", andForm},
    {"or", orForm},
    {"lambda", lambdaForm},
    {"cond", condForm},
    {"let", letForm},
    {"begin", beginForm},
    
};

ValuePtr defineForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (args[0]->isType(ValueType::PAIR)) {

        auto pair = static_cast<PairValue*>(args[0].get());
        std::string name = pair->getCar()->toString();
        std::vector<ValuePtr> lambdaArgs{pair->getCdr()};
        lambdaArgs.insert(lambdaArgs.end(), args.begin() + 1, args.end());
        ValuePtr lambda = lambdaForm(lambdaArgs, env);
        env.define(name, lambda);
        return std::make_shared<NilValue>();
    
    } else {

        if (args.size() != 2) {
            throw LispError("define requires exactly two arguments.");
        }
        if (auto name = args[0]->asSymbol()) {
            env.define(*name, env.eval(args[1]));
            return std::make_shared<NilValue>();
        } else {
            throw LispError("Unimplemented.");
        }

    }
}

ValuePtr quoteForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (args.size() != 1) {
        throw LispError("quote requires exactly one argument.");
    }
    return args[0];
}

ValuePtr quasiquote(const ValuePtr& arg, EvalEnv& env) {
    if (arg->isType(ValueType::PAIR)) {
        if (auto symbol = arg->toVector()[0]->asSymbol()) {
            if (symbol.value() == "unquote" || symbol.value() == ",") {
                if (arg->toVector().size() != 2) {
                    throw LispError("unquote requires exactly one argument.");
                }

                return env.eval(arg->toVector()[1]);
            }
        } else {
            std::vector<ValuePtr> result;
            for (const auto& element : arg->toVector()) {
                result.push_back(quasiquote(element, env));
            }
            return std::make_shared<PairValue>(result);
        }
    } else {
        return arg;
    }
    return nullptr;
}

ValuePtr quasiquoteForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (args.size() != 1) {
        throw LispError("quasiquote requires exactly one argument.");
    }
    return quasiquote(args[0], env);
}

ValuePtr ifForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (args.size() != 2 && args.size() != 3) {
        throw LispError("if requires exactly two or three arguments.");
    }

    ValuePtr pred = env.eval(args[0]);
    if (pred->asBoolean() == false) {
        if (args.size() == 3) {
            return env.eval(args[2]);
        } else {
            return std::make_shared<NilValue>();
        }
    } else {
        return env.eval(args[1]);
    }
}

ValuePtr andForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (args.size() == 0) {
        return std::make_shared<BooleanValue>(true);
    }
    if (args.size() < 2) {
        throw LispError("and requires at least two arguments.");
    }

    ValuePtr lastValue;
    for (size_t i = 0; i < args.size(); i++) {
        lastValue = env.eval(args[i]);
        if (!lastValue->asBoolean()) {
            return lastValue;
        }
    }

    return lastValue;
}

ValuePtr orForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (args.size() == 0) {
        return std::make_shared<BooleanValue>(false);
    }
    if (args.size() < 2) {
        throw LispError("and requires at least two arguments.");
    }

    ValuePtr lastValue;
    for (size_t i = 0; i < args.size(); i++) {
        lastValue = env.eval(args[i]);
        if (lastValue->asBoolean()) {
            return lastValue;
        }
    }

    return lastValue;
}

ValuePtr lambdaForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (args.size() < 2) {
        throw LispError("lambda requires at least two arguments.");
    } 

    std::vector<std::string> params;
    std::ranges::transform(
        args[0]->toVector(), 
        std::back_inserter(params),
        [](ValuePtr v) { return v->toString(); }
    );
    std::vector<ValuePtr> body{args.begin() + 1, args.end()};

    return std::make_shared<LambdaValue>(
        params,
        body, 
        env.shared_from_this()
    );
}


ValuePtr condForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    for (const auto& p : args) {
        auto arg = static_cast<PairValue*>(p.get());
        if (arg->toVector().empty()) {
            throw LispError("empty clause in cond.");
        }

        ValuePtr result;
        if (arg->getCar()->asSymbol() == "else") {
            if (&p != &args.back()) {
                throw LispError("else clause is not the last clause in cond.");
            }
            for (size_t i = 1; i < arg->toVector().size(); i++) {
                result = env.eval(arg->toVector()[i]);
            }
            if (result) {
                return result;
            } else {
                throw LispError("there must be expressions after else.");
            }
        }

        ValuePtr condition = env.eval(arg->getCar());
        if (condition->asBoolean()) {
            for (size_t i = 1; i < arg->toVector().size(); i++) {
                result = env.eval(arg->toVector()[i]);
            }
            return result ? result : condition;
        }
    }

    throw LispError("No true clause in cond.");
}

ValuePtr letForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (args.size() < 2) {
        throw LispError("Let form requires at least two arguments.");
    }

    std::vector<std::string> identifiers;
    std::vector<ValuePtr> initialValues;
    for (const auto& binding : args[0]->toVector()) {
        if (binding->toVector().size() != 2 || !binding->toVector()[0]->isType(ValueType::SYMBOL)) {
            throw LispError("Invalid binding in let form.");
        }

        identifiers.push_back(binding->toVector()[0]->asSymbol().value());
        initialValues.push_back(env.eval(binding->toVector()[1]));
    }

    auto childEnv = env.createChild(identifiers, initialValues);

    ValuePtr result;
    for (size_t i = 1; i < args.size(); i++) {
        result = childEnv->eval(args[i]);
    }

    return result;
}

ValuePtr beginForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (args.size() == 0) {
        return std::make_shared<NilValue>();
    }

    ValuePtr result;
    for (size_t i = 0; i < args.size(); i++) {
        result = env.eval(args[i]);
    }

    return result;
}
