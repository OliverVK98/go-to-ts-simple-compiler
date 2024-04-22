package main

import "fmt"

func add(x, y int) int {
	return x + y
}

// Test comment

func checkPositive(num int) bool {
	return num > 0
}

func main() {
	var a int = 10
	var str string = "Hello"
	const flag bool = true
	var b int = 42

	var (
		g string = "Grouped"
		h bool   = true
		c int    = 100
	)

	j := []int{1, 2, 3, 4, 5}

	fmt.Println("Initial values:", a, str, b, g, h, c, j)

	result := add(a, b)
	fmt.Println("Result of add(a, b):", result)

	if flag != true {
		fmt.Println("Flag is false, unexpected.")
	} else {
		fmt.Println("Flag is true, as expected:", flag)
	}

	if checkPositive(result) {
		fmt.Println("Result is positive. Setting result to 10.")
		result = 10
	} else {
		fmt.Println("Result is not positive. Setting result to 20.")
		result = 20
	}

	fmt.Println("Final value of result:", result)
	fmt.Println("Values of multiple variables:", str, b, g, h, j)

	result = add(c, b)
	fmt.Println("Result of add(c, b):", result)
	fmt.Println("Checking positivity of new result:")
	if checkPositive(result) {
		fmt.Println("New result is positive.")
	} else {
		fmt.Println("New result is not positive.")
	}
}
