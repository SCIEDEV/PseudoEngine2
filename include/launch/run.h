#pragma once
#include "lexer/lexer.h"
#include "parser/parser.h"
#include <filesystem>

bool runFile(std::filesystem::path &filename);

bool startREPL();
