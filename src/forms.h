#ifndef FORMS_H
#define FORMS_H

#include <vector>

#include "./eval_env.h"
#include "./value.h"

using SpecialFormType = ValuePtr(const std::vector<ValuePtr>&, EvalEnv&);

ValuePtr defineForm(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr quoteForm(const std::vector<ValuePtr>& params, EvalEnv& env);

extern std::unordered_map<std::string, SpecialFormType*> SPECIAL_FORMS;

#endif
