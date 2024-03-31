#include <iostream>
#include "lexer/lexer.h"
#include "token/token.h"
#include "parser/parser.h"

std::string testCase = "func add(a, b) { return a + b; }";

std::unordered_map<std::string, std::string> testCases = {
        {"const a = 2;", "ConstStatement(Identifier(a) = Integer(2))"},
        {"const b = \"test string\";", "ConstStatement(Identifier(b) = String(test string))"},
        {"true;", "ExpressionStatement(Boolean(true))"},
        {"\"drug\"", "ExpressionStatement(String(drug))"},
        {"return 5 + 6;", "ReturnStatement((Integer(5) + Integer(6)))"},
        {"return;", "ReturnStatement(EMPTY)"},
        {"if (2>1) {return true;}", "IfStatement(Condition((Integer(2) > Integer(1))) Consequence(ReturnStatement(Boolean(true))) Alternative())"},
        {"func add(a, b) { return a + b; }", "Function(Name(Identifier(add)) Params(Identifier(a), Identifier(b)) Body(ReturnStatement((Identifier(a) + Identifier(b)))))"},
        {"const a = func add(a, b) { return a + b; }", "ConstStatement(Identifier(a) = Function(Name(a) Params(Identifier(a), Identifier(b)) Body(ReturnStatement((Identifier(a) + Identifier(b))))))"},
        {"add(4, 3)", "ExpressionStatement(FunctionCall(Identifier(add)(Integer(4), Integer(3))))"},
        {"[1, 2, 3][1]", "ExpressionStatement(IndexExpression(Left:(Array([Integer(1), Integer(2), Integer(3)])) Right: [Integer(1)]))"}
};

void runTestCases() {
    int totalTests = 0;
    int passedTests = 0;

    for (const auto& testCase : testCases) {
        const std::string& testInput = testCase.first;
        const std::string& expectedOutput = testCase.second;
        totalTests++;

        Lexer newLexer(testInput);
        Parser newParser{&newLexer};
        Program output = newParser.parseProgram();
        std::string actualOutput = output.string();

        std::cout << "Test input: " << testInput << std::endl;
        std::cout << "Expected output: " << expectedOutput << " (Length: " << expectedOutput.length() << ")" << std::endl;
        std::cout << "Actual output: " << actualOutput << " (Length: " << actualOutput.length() << ")" << std::endl;

        if (actualOutput == expectedOutput) {
            passedTests++;
            std::cout << "Test passed" << std::endl;
        } else {
            std::cout << "Test failed" << std::endl;
        }
        std::cout << std::endl; // Newline for readability
    }

    std::cout << passedTests << "/" << totalTests << " tests passed.";
}

void lexTestCase() {
    Lexer newLexer(testCase);
    for (auto currTok = newLexer.nextToken(); currTok.Type != END_OF_FILE; currTok = newLexer.nextToken()) {
        std::cout << currTok << std::endl;
    }
}

int main() {
    runTestCases();
//    lexTestCase();
    return 0;
}

