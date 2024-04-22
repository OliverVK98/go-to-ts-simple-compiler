function add(y: number, x: number): number {
	return x + y;
}
function checkPositive(num: number): boolean {
	return num > 0;
}
function main(): void {
	let a: number = 10;
	let str: string = "Hello";
	const flag: boolean = true;
	let b: number = 42;
		let g: string = "Grouped";
	let h: boolean = true;
	let c: number = 100;
	let j: number[] = [1, 2, 3, 4, 5];
	console.log("Initial values:", a, str, b, g, h, c, j);
	let result: number = add(a, b);
	console.log("Result of add(a, b):", result);
	if (flag != true) {
		console.log("Flag is false, unexpected.");
	} else {
		console.log("Flag is true, as expected:", flag);
	}
	if (checkPositive(result)) {
		console.log("Result is positive. Setting result to 10.");
		result = 10;
	} else {
		console.log("Result is not positive. Setting result to 20.");
		result = 20;
	}
	console.log("Final value of result:", result);
	console.log("Values of multiple variables:", str, b, g, h, j);
	result = add(c, b);
	console.log("Result of add(c, b):", result);
	console.log("Checking positivity of new result:");
	if (checkPositive(result)) {
		console.log("New result is positive.");
	} else {
		console.log("New result is not positive.");
	}
}
