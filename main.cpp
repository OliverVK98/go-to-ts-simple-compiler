#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "compiler/compiler.h"

std::string preprocessInputFile(const std::string& filename) {
    std::ifstream file(filename);
    std::string line;
    std::stringstream ss;

    if (!file.is_open()) {
        throw std::runtime_error("Could not open the file: " + filename);
    }

    while (getline(file, line)) {
        if (line.find("package") == 0 || line.find("import") == 0 || line.find("//") == 0) {
            continue;
        }
        ss << line << '\n';
    }

    file.close();
    return ss.str();
}

void compileInputFile(const std::string& input) {
    Lexer newLexer(input);
    Parser newParser{&newLexer};
    auto output = newParser.parseProgram();
    Compiler compiler("./output.ts");
    compiler.compile(std::move(output));
}

int main() {
    std::string filename = "input.go";
    try {
        std::string processedInput = preprocessInputFile(filename);
        compileInputFile(processedInput);
    } catch (std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}
