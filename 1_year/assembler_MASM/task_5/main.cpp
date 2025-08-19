#include <stdio.h> 

int a, b, c, d, sum, notsum, result; 

int main() {
	a = 3; 
	b = 2; 
	c = 1; 
	d = 7; 
	result = sum = 0; 
	_asm {
		mov eax, a
		add eax, b 

		mov sum, eax 
		add eax, c 
		mov sum, eax 
		sub d, 5 
		mov eax, sum 
		mov ecx, d 
		sub edx, edx 
		div ecx 

		mov result, eax 
	}
	printf("%d", result); 

	return 0; 
}