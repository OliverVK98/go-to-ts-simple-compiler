#include <iostream>
#include "lexer/lexer.h"
#include "token/token.h"
#include "parser/parser.h"
#include "compiler/compiler.h"

std::string testCase = R"(
func test(a, b string, c []int) string {
	var newString = a + 20

    func test(a, b int, c []int) string {
	    var newString = a + 20
        func test(a, b bool, c []int) string {
            var newString = a + 20
        }
    }

    return newString
}
)";

std::string variableDeclTestCase = R"(
a := 5;
nums := []int{1, 2, 3}
flag := true

const (
    Name string = "Go"
    Version = "1.16"
    ReleasedYear int = 2009
    TrueFlag bool = true
    FalseFlag = false
)

const d = 5
const b bool = false

var (
    explicitInt int = 10
    implicitInt = 20
    noValueInt int

    explicitBool bool = true
    explicitString string = "explicit"

    explicitArray [...]int = [...]int{1, 2, 3, 4, 5}
    implicitArray = [5]int{6, 7, 8, 9, 10}
    noValueArray [5]int
)

var intVar int = 5
var boolVar = false
var stringVar string = "hello"
var uninitString string

var boolArrayVar [2]bool = [2]bool{true, false}
var arr = [5]int{1, 2, 3, 4, 5}
var uninitIntArray [3]int
)";

std::unordered_map<std::string, std::string> testCases = {
        {"const a = 2;", "ConstNode(Identifier(a) = Integer(2))"},
        {"const b = \"test testString\";", "ConstNode(Identifier(b) = String(test testString))"},
        {"true;", "RValue(Boolean(true))"},
        {"\"drug\"", "RValue(String(drug))"},
        {"return 5 + 6;", "ReturnNode((Integer(5) + Integer(6)))"},
        {"return;", "ReturnNode(EMPTY)"},
        {"if (2>1) {return true;}", "IfStatement(Condition((Integer(2) > Integer(1))) Consequence(ReturnNode(Boolean(true))) Alternative())"},
        {"funcName add(a, b) { return a + b; }", "Function(Name(Identifier(add)) Params(Identifier(a), Identifier(b)) Body(ReturnNode((Identifier(a) + Identifier(b)))))"},
        {"const a = funcName add(a, b) { return a + b; }", "ConstNode(Identifier(a) = Function(Name(a) Params(Identifier(a), Identifier(b)) Body(ReturnNode((Identifier(a) + Identifier(b))))))"},
        {"add(4, 3)", "RValue(FunctionCall(Identifier(add)(Integer(4), Integer(3))))"},
        {"[1, 2, 3][1]", "RValue(IndexExpression(Left:(Array([Integer(1), Integer(2), Integer(3)])) Right: [Integer(1)]))"},
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
    Compiler compiler("./output.ts");
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

