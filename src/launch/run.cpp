#include "pch.h"
#include <fstream>
#include <filesystem>

#include "launch/run.h"

bool runFile(std::filesystem::path &filename) {
    std::ifstream fd(filename.c_str());

    if (!fd.is_open()) {
        std::cerr << "error: fd '" << filename << "' not found!" << std::endl;
        return false;
    }

    std::string contents;
	try {
		contents.reserve(std::filesystem::file_size(filename));	
	} catch (std::filesystem::filesystem_error &error) {
		std::cerr << "error reading file: " << error.what() << error.code();
		return false;
	}
	
    while (fd.good()) {
        std::string line;
        std::getline(fd, line);
		contents += line;
		contents += "\n";
    }
    fd.close();

    Lexer lexer(&contents);
    try {
        const std::vector<Token*> &tokens = lexer.makeTokens();
        Parser parser(&tokens);
        Interpreter::Block *block = parser.parse();
        std::cout.precision(10);

        auto globalCtx = Interpreter::Context::createGlobalContext();
        block->run(*globalCtx);
    } catch (const Interpreter::Error &e) {
        std::cout << "\n";
        e.print();
        return false;
    }
    return true;
}
