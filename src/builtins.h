#ifndef BUILTINS_H
#define BUILTINS_H

#include <vector>
#include <memory>
#include <unordered_map>

#include "./value.h"

namespace builtins {

void init();

// core library
extern std::unordered_map<std::string, std::shared_ptr<BuiltinProcValue> > core_builtins;

ValuePtr apply(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr display(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr displayln(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr error(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr eval(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr exit(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr newline(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr print(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr readline(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr help(const std::vector<ValuePtr>& params, EvalEnv& env);

// type checking library
extern std::unordered_map<std::string, std::shared_ptr<BuiltinProcValue> > type_checking_builtins;

ValuePtr isAtom(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr isBoolean(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr isInteger(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr isList(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr isNumber(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr isNull(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr isPair(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr isProcedure(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr isString(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr isSymbol(const std::vector<ValuePtr>& params, EvalEnv& env);

// cons_list library

extern std::unordered_map<std::string, std::shared_ptr<BuiltinProcValue> > cons_list_builtins;

ValuePtr car(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr cdr(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr cons(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr length(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr list(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr append(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr map(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr filter(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr reduce(const std::vector<ValuePtr>& params, EvalEnv& env);

// math library
extern std::unordered_map<std::string, std::shared_ptr<BuiltinProcValue> > math_builtins;

ValuePtr addVal(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr subVal(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr mulVal(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr divVal(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr absVal(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr exptVal(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr quotientVal(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr remainderVal(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr moduloVal(const std::vector<ValuePtr>& params, EvalEnv& env);

// comparison library
extern std::unordered_map<std::string, std::shared_ptr<BuiltinProcValue> > comparison_builtins;

ValuePtr dataEq(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr dataEqual(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr notVal(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr eqVal(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr ltVal(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr gtVal(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr leVal(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr geVal(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr isEven(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr isOdd(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr isZero(const std::vector<ValuePtr>& params, EvalEnv& env);

// builtin library
extern std::unordered_map<std::string, std::shared_ptr<BuiltinProcValue> > builtins;

}

#endif