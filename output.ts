function main(): void {
	let a: number = 10;
	let str: string = "Hello";
	const flag: boolean = true;
	let d: number = 42;
	let g: string = "Grouped";
	let h: boolean = true;
	let b: number = 100;
	let j: number[] = [1, 2, 3, 4, 5];
	function add(y: number, x: number): number {
		return x + y;
	}
	function checkPositive(num: number): boolean {
		return num > 0;
	}
	let result: number = add(a, b);
	if (flag != true) {
		result = 10;
	} else {
		result = 20;
	}
	if (checkPositive(result)) {
		result = 10;
	} else {
		result = 20;
	}
}
