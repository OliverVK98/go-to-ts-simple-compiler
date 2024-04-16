#include <iostream>
#include "lexer/lexer.h"
#include "token/token.h"
#include "parser/parser.h"
#include "compiler/compiler.h"

std::string testCase = R"(
const a int = 10
func test(a, b string, c []int) string {
	var newString = 1 + 1 + 2
	var newString = 1 + 1
	var newString = a
	var newString = a + "test"
	var newString = a + "test" + "test"

    func test(a, b int, c []int) string {
	    var newString = 1 + 1 + 2
	    var newString = 1 + 1
	    var newString = a
	    var newString = a + "test"
	    var newString = a + "test" + "test"
    }

    return
}
var a = test(a, b, "string", 1, []int{1,2,3}) + 3 + 3 + 3 + 3

)";

std::string functionAndCallsTestCase = R"(
const a int = 10
func test(a, b string, c []int) string {
	var newString = 1 + 1 + 2
	var newString = 1 + 1
	var newString = a
	var newString = a + "test"
	var newString = a + "test" + "test"

    func test(a, b int, c []int) string {
	    var newString = 1 + 1 + 2
	    var newString = 1 + 1
	    var newString = a
	    var newString = a + "test"
	    var newString = a + "test" + "test"
    }

    return
}
var a = test(a, b, "string", 1, []int{1,2,3}) + 3

)";

std::string variableDeclTestCase = R"(
a := 5
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
//    lexTestCase();
        compileTestCase();
    } catch (std::runtime_error& e) {
        std::cerr << e.what();
    }

    return 0;
}

