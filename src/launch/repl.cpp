#include "pch.h"

#include <string>
#include "launch/run.h"

extern bool REPLMode;

static const std::string_view multilineKeywords[] = {
    "IF",
    "CASE",
    "WHILE",
    "REPEAT",
    "FOR",
    "PROCEDURE",
    "FUNCTION",
    "TYPE"
};

bool startREPL() {
    std::cout << "scie.dev pseudocode compiler repl. enter 'EXIT' to quit";

    Lexer lexer;
    Parser parser;
    auto globalCtx = Interpreter::Context::createGlobalContext();

    while (true) {
        std::cout << "> " << std::flush;
        std::string input;
        std::getline(std::cin, input);

        size_t size = input.size();
        if (input.empty()) continue;
        if (input == "EXIT") break;
        if (input.starts_with("INCLUDE")) {
            if (size < 9) {
                std::cerr << "expected filename: INCLUDE <filename>" << std::endl;
                continue;
            }
            std::filesystem::path psfilename = input.substr(8, size - 8);
            REPLMode = false;
            
            std::cout << "running file" << psfilename << "\n";
			std::string_view statusMessage = runFile(psfilename) ? "success\n" : "error\n";
            std::cout << "\nprogram exited " << statusMessage << "\n";
            
            psfilename = "<stdin>";
            REPLMode = true;
            continue;
        }

        for (const std::string_view &keyword : multilineKeywords) {
            if (input.starts_with(keyword)) {
                if (keyword == "TYPE" && input.find("=") != std::string::npos) break;
                std::string line = " ";
                while (line.size() > 0) {
					input += "\n";
                    std::cout << ". " << std::flush;
                    std::getline(std::cin, line);
					input += line;
                }
                break;
            }
        }

        lexer.setExpr(&input);
        try {
            const std::vector<Token*> &tokens = lexer.makeTokens();
            parser.setTokens(&tokens);
            Interpreter::Block *block = parser.parse();

            std::cout.precision(10);
            block->run(*globalCtx);
        } catch (const Interpreter::Error &e) {
            std::cout << "\n";
            e.print();
        }
    }
    return true;
}
