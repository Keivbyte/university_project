#include <stdio.h>

int a; 

int main() {
	a = 10; 
	_asm
	{
		add a, 10 
		sub a, 2 
	}
	printf("%d\n", a); 
	return 0; 
}
