#include "./error.h"
#include "./parser.h"

Parser::Parser(std::deque<TokenPtr> tokens) : tokens{std::move(tokens)} {}

ValuePtr Parser::createQuote(std::string quoteName) {
    return std::make_shared<PairValue>(
        std::make_shared<SymbolValue>(quoteName),
        std::make_shared<PairValue>(
            this->parse(),
            std::make_shared<NilValue>()
        )
    );
}

ValuePtr Parser::parse() {
    auto token = std::move(tokens.front());
    tokens.pop_front(); // after std::move, the token is no longer valid, so we can't use it anymore

    switch (token->getType()) {
    case TokenType::BOOLEAN_LITERAL:
    {
        auto value = static_cast<BooleanLiteralToken&>(*token).getValue();
        return std::make_shared<BooleanValue>(value);
        break;
    }
    
    case TokenType::NUMERIC_LITERAL:
    {
        auto value = static_cast<NumericLiteralToken&>(*token).getValue();
        return std::make_shared<NumericValue>(value);
        break;
    }
    
    case TokenType::STRING_LITERAL:
    {
        auto value = static_cast<StringLiteralToken&>(*token).getValue();
        return std::make_shared<StringValue>(value);
        break;
    }
    
    case TokenType::IDENTIFIER:
    {
        auto name = static_cast<IdentifierToken&>(*token).getName();
        return std::make_shared<SymbolValue>(name);
        break;
    }

    case TokenType::LEFT_PAREN:
    {
        return this->parseTails();
        break;
    }

    case TokenType::QUOTE:
    {
        return this->createQuote("quote");
        break;
    }

    case TokenType::QUASIQUOTE:
    {
        return this->createQuote("quasiquote");
        break;
    }

    default:
        throw SyntaxError("Unimplemented.");
        break;
    }
}

ValuePtr Parser::parseTails() {
    if (tokens.front()->getType() == TokenType::RIGHT_PAREN) {
        tokens.pop_front();
        return std::make_shared<NilValue>();
    }

    auto car = this->parse();
    if(tokens.front()->getType() == TokenType::DOT) {
        tokens.pop_front(); // pop '.'
        auto cdr = this->parse();
        if(tokens.front()->getType() != TokenType::RIGHT_PAREN) {
            throw SyntaxError("Expected ')'.");
        } // check if the next token is ')'
        tokens.pop_front(); // pop ')'
        return std::make_shared<PairValue>(car, cdr);
    } else {
        auto cdr = this->parseTails();
        return std::make_shared<PairValue>(car, cdr);
    }
}