#include <fstream>
#include <iostream>
#include <string>

#include "./eval_env.h"
#include "./parser.h"
#include "./tokenizer.h"
#include "./value.h"

#include "rjsj_test.hpp"

void process(std::istream& input, bool print_result, std::vector<std::string> args = {"0", ""}) {
    std::shared_ptr<EvalEnv> env = std::make_shared<EvalEnv>();
    args[0] = "(define argc " + args[0] + ")";
    args[1] = "(define argv (list" + args[1] + "))";
    for (auto line : args){ // Define argc and argv
        auto tokens = Tokenizer::tokenize(line);
        Parser parser(std::move(tokens));
        auto value = parser.parse();
        env->eval(std::move(value));
    }
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
                if (line.empty() || line[0] == ';') {
                    continue;
                }
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

std::string parseArgs(std::vector<std::string> args) {
    std::string result = "";
    for (int i = 0; i < args.size(); i++) {
        result += "\"" + args[i] + "\"";
        if (i != args.size() - 1) {
            result += " ";
        }
    }
    return result;
}

struct TestCtx {
    std::shared_ptr<EvalEnv> env = std::make_shared<EvalEnv>();
    std::string eval(std::string input) {
        auto tokens = Tokenizer::tokenize(input);
        Parser parser(std::move(tokens));
        auto value = parser.parse();
        return env->eval(std::move(value))->toString();
    }
};

int main(int argc, char* argv[]) {
    // RJSJ_TEST(TestCtx, Lv2, Lv3, Lv4, Lv5, Lv5Extra, Lv6, Lv7, Lv7Lib, Sicp);
    if (argc == 1) {
        // REPL mode
        std::cout << "Welcome to Mini-Lisp Interpreter v1.0.0" << std::endl;
        std::cout << "Type \"(help)\" for more information, \"(exit n)\" to exit with code n."<< std::endl;
        process(std::cin, true);
    } else {
        // File mode
        std::vector<std::string> args;
        if (argc > 2) {
            std::string result(argv[1]);
            size_t pos = 0;
            while ((pos = result.find('\\', pos)) != std::string::npos) {
                result.insert(pos, "\\");
                pos += 2; 
            }
            args.push_back(result);
            for (int i = 2; i < argc; i++) {
                args.push_back(std::string(argv[i]));
            }
        }
        std::ifstream file(argv[1]);
        if (!file) {
            std::cerr << "Could not open file " << argv[1] << std::endl;
            return 1;
        }
        if (args.empty()) {
            process(file, false);
        } else {
            process(file, false, std::vector({std::to_string(args.size()), parseArgs(args)}));
        }
    }
    return 0;
}
