#include <algorithm>
#include <ranges>

#include "./error.h"
#include "./eval_env.h"
#include "./forms.h"

std::unordered_map<std::string, SpecialFormType*> SPECIAL_FORMS = {

    {"define", defineForm},
    {"quote", quoteForm},
    // {"quasiquote", quasiquoteForm},
    // {"if", ifForm},
    // {"and", andForm},
    // {"or", orForm},
    // {"lambda", lambdaForm},
    // {"cond", condForm},
    // {"let", letForm},
    // {"begin", beginForm},
    
};

ValuePtr defineForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (args.size() != 2) {
        throw LispError("define requires exactly two arguments.");
    }
    if (auto name = args[0]->asSymbol()) {
        env.define(name.value(), env.eval(args[1]));
        return std::make_shared<NilValue>();
    } else {
        throw LispError("Unimplemented.");
    }
}

ValuePtr quoteForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (args.size() != 1) {
        throw LispError("quote requires exactly one argument.");
    }
    return args[0];
}

