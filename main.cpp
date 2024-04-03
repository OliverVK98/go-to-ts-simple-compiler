#include <iostream>
#include <fstream>
#include "lexer/lexer.h"
#include "token/token.h"
#include "parser/parser.h"
#include "compiler/compiler.h"

std::string testCase2 = R"(var arr = [5]int{1, 2, 3, 4, 5}; var arr [5]int = [5]int{1, 2, 3, 4, 5}; var arr [5]int; var b int; var b int = 5; var c bool; var c bool = true; var d string; var d string = "hello";)";
std::string testCase = R"(
var (
    explicitInt int = 10
    implicitInt = 20
    noValueInt int

    explicitBool bool = true
    implicitBool = false
    noValueBool bool

    explicitString string = "explicit"
    implicitString = "implicit"
    noValueString string
)
)";
std::string testCase1 = R"(
var (
    explicitInt int = 10
    implicitInt = 20
    noValueInt int

    explicitBool bool = true
    implicitBool = false
    noValueBool bool

    explicitString string = "explicit"
    implicitString = "implicit"
    noValueString string

    explicitArray [5]int = [5]int{1, 2, 3, 4, 5}
    implicitArray = [5]int{6, 7, 8, 9, 10}
    noValueArray [5]int
)
)";
std::string testCase3 = R"(
const (
    Name string = "Go"
    Version = "1.16"
    ReleasedYear int = 2009
    TrueFlag bool = true
    FalseFlag = false
)

var intVar1 int = 5
var intVar2 = 10
var boolVar1 bool = true
var boolVar2 = false
var stringVar1 string = "hello"
var stringVar2 = "world"

var intArrayVar [5]int = [5]int{1, 2, 3, 4, 5}
var boolArrayVar [2]bool = [2]bool{true, false}
var stringArrayVar [3]string = [3]string{"a", "b", "c"}

// Variables without initial values
var uninitInt int
var uninitBool bool
var uninitString string
var uninitIntArray [3]int
)";

std::unordered_map<std::string, std::string> testCases = {
        {"const a = 2;", "ConstNode(Identifier(a) = Integer(2))"},
        {"const b = \"test testString\";", "ConstNode(Identifier(b) = String(test testString))"},
        {"true;", "RHValue(Boolean(true))"},
        {"\"drug\"", "RHValue(String(drug))"},
        {"return 5 + 6;", "ReturnNode((Integer(5) + Integer(6)))"},
        {"return;", "ReturnNode(EMPTY)"},
        {"if (2>1) {return true;}", "IfStatement(Condition((Integer(2) > Integer(1))) Consequence(ReturnNode(Boolean(true))) Alternative())"},
        {"func add(a, b) { return a + b; }", "Function(Name(Identifier(add)) Params(Identifier(a), Identifier(b)) Body(ReturnNode((Identifier(a) + Identifier(b)))))"},
        {"const a = func add(a, b) { return a + b; }", "ConstNode(Identifier(a) = Function(Name(a) Params(Identifier(a), Identifier(b)) Body(ReturnNode((Identifier(a) + Identifier(b))))))"},
        {"add(4, 3)", "RHValue(FunctionCall(Identifier(add)(Integer(4), Integer(3))))"},
        {"[1, 2, 3][1]", "RHValue(IndexExpression(Left:(Array([Integer(1), Integer(2), Integer(3)])) Right: [Integer(1)]))"},
        {"const a = 10; const b = 12;", "123"}
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
        auto output = newParser.parseProgram();
        std::string actualOutput = output->testString();

        std::cout << "Test input: " << testInput << std::endl;
        std::cout << "Expected output: " << expectedOutput << " (Length: " << expectedOutput.length() << ")" << std::endl;
        std::cout << "Actual output: " << actualOutput << " (Length: " << actualOutput.length() << ")" << std::endl;

        if (actualOutput == expectedOutput) {
            passedTests++;
            std::cout << "Test passed" << std::endl;
        } else {
            std::cout << "Test failed" << std::endl;
        }
        std::cout << std::endl;
    }

    std::cout << passedTests << "/" << totalTests << " tests passed.";
}

void lexTestCase() {
    Lexer newLexer(testCase);
    for (auto currTok = newLexer.nextToken(); currTok.Type != END_OF_FILE; currTok = newLexer.nextToken()) {
        std::cout << currTok << std::endl;
    }
}

void compileTestCase() {
    Lexer newLexer(testCase);
    Parser newParser{&newLexer};
    auto output = newParser.parseProgram();
    Compiler compiler("./compilerTest.txt");
    compiler.compile(std::move(output));
}

int main() {
    try {
//            runTestCases();
//    lexTestCase();
        compileTestCase();
    } catch (std::runtime_error& e) {
        std::cerr << e.what();
    }

    return 0;
}

