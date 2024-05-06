#ifndef PARSER_H
#define PARSER_H

#include <deque>

#include "./token.h"
#include "./value.h"

class Parser {
    std::deque<TokenPtr> tokens;
public:
    Parser(std::deque<TokenPtr> tokens);

    ValuePtr createQuote(std::string quoteName);

    ValuePtr parseTails();
    ValuePtr parse();
};

#endif