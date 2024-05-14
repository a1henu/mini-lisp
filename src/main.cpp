#include <fstream>
#include <iostream>
#include <string>

#include "./eval_env.h"
#include "./parser.h"
#include "./tokenizer.h"
#include "./value.h"

#include "rjsj_test.hpp"

void process(std::istream& input, bool print_result) {
    std::shared_ptr<EvalEnv> env = std::make_shared<EvalEnv>();
    while (true) {
        try {
            if (print_result) {
                std::cout << "\033[31m" << ">>> " << "\033[0m";
            }
            std::string line;
            std::getline(input, line);
            // More than one line
            // While the number of opening parentheses is greater than the number of closing parentheses
            while (std::count(line.begin(), line.end(), '(') > std::count(line.begin(), line.end(), ')')) {
                int n = std::count(line.begin(), line.end(), '(') - std::count(line.begin(), line.end(), ')');
                if (print_result) {
                    std::cout << "...";
                    for (int i = 0; i < n; i++) {
                        std::cout << "    ";
                    }
                }
                std::string extraLine;
                std::getline(input, extraLine);
                line += "\n" + extraLine;
            }
            if (!input.eof() || !line.empty()) {
                auto tokens = Tokenizer::tokenize(line);
                Parser parser(std::move(tokens));
                auto value = parser.parse();
                auto result = env->eval(std::move(value));
                if (print_result) {
                    std::cout << result->toString() << std::endl;
                } 
            } else {
                std::exit(0);
            }
            
        } catch (std::runtime_error& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc == 1) {
        // REPL mode
        process(std::cin, true);
    } else {
        // File mode
        std::ifstream file(argv[1]);
        if (!file) {
            std::cerr << "Could not open file " << argv[1] << std::endl;
            return 1;
        }
        process(file, false);
    }
    return 0;
}
