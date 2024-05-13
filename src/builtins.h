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

ValuePtr apply(const std::vector<ValuePtr>& params);
ValuePtr display(const std::vector<ValuePtr>& params);
ValuePtr exit(const std::vector<ValuePtr>& params);
ValuePtr newline(const std::vector<ValuePtr>& params);
ValuePtr print(const std::vector<ValuePtr>& params);

// type checking library
extern std::unordered_map<std::string, std::shared_ptr<BuiltinProcValue> > type_checking_builtins;

ValuePtr isAtom(const std::vector<ValuePtr>& params);
ValuePtr isBoolean(const std::vector<ValuePtr>& params);
ValuePtr isInteger(const std::vector<ValuePtr>& params);
ValuePtr isList(const std::vector<ValuePtr>& params);
ValuePtr isNumber(const std::vector<ValuePtr>& params);
ValuePtr isNull(const std::vector<ValuePtr>& params);
ValuePtr isPair(const std::vector<ValuePtr>& params);
ValuePtr isProcedure(const std::vector<ValuePtr>& params);
ValuePtr isString(const std::vector<ValuePtr>& params);
ValuePtr isSymbol(const std::vector<ValuePtr>& params);

// cons_list library

extern std::unordered_map<std::string, std::shared_ptr<BuiltinProcValue> > cons_list_builtins;

ValuePtr car(const std::vector<ValuePtr>& params);
ValuePtr cdr(const std::vector<ValuePtr>& params);
ValuePtr cons(const std::vector<ValuePtr>& params);
ValuePtr length(const std::vector<ValuePtr>& params);
ValuePtr list(const std::vector<ValuePtr>& params);
ValuePtr append(const std::vector<ValuePtr>& params);

// math library
extern std::unordered_map<std::string, std::shared_ptr<BuiltinProcValue> > math_builtins;

ValuePtr addVal(const std::vector<ValuePtr>& params);
ValuePtr subVal(const std::vector<ValuePtr>& params);
ValuePtr mulVal(const std::vector<ValuePtr>& params);
ValuePtr divVal(const std::vector<ValuePtr>& params);
ValuePtr absVal(const std::vector<ValuePtr>& params);
ValuePtr exptVal(const std::vector<ValuePtr>& params);
ValuePtr quotientVal(const std::vector<ValuePtr>& params);
ValuePtr remainderVal(const std::vector<ValuePtr>& params);
ValuePtr moduloVal(const std::vector<ValuePtr>& params);

// comparison library
extern std::unordered_map<std::string, std::shared_ptr<BuiltinProcValue> > comparison_builtins;

ValuePtr eqVal(const std::vector<ValuePtr>& params);
ValuePtr ltVal(const std::vector<ValuePtr>& params);
ValuePtr gtVal(const std::vector<ValuePtr>& params);
ValuePtr leVal(const std::vector<ValuePtr>& params);
ValuePtr geVal(const std::vector<ValuePtr>& params);
ValuePtr isEven(const std::vector<ValuePtr>& params);
ValuePtr isOdd(const std::vector<ValuePtr>& params);
ValuePtr isZero(const std::vector<ValuePtr>& params);

// builtin library
extern std::unordered_map<std::string, std::shared_ptr<BuiltinProcValue> > builtins;

}

#endif