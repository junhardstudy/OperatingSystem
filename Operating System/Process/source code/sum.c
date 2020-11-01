#include <stdio.h>
#include <stdlib.h>

void main(int argc, char* argv[]){
	/***************************************************
	 This function take two arguments wich is converted
	 char into int.
	 Two int variables are sum, and the result is
	 displayed by printf().
	 ***************************************************/
	int first, second, result;

	first = atoi(argv[1]);
	second = atoi(argv[2]);
	result = first + second;

	printf("%d + %d = %d\n", first, second, result);
}

