#include <stdio.h>
#include <stdlib.h>

int fact_cal(int);

void main(int argc, char* argv[]){
	/************************************************************
	 This function take one arguments wich is converted char into
	 integer.
	 And the integer is calculated by fact_calc() which is
	 performing factorial.
	 The result is displayed by printf()
	*************************************************************/
	int number, result;

	number = atoi(argv[1]);
	if(number < 0){
		printf("minus factorial can't be calculate!, program will be terminated\n");
		exit(-1);
	}
	result = fact_cal(number);
	
	printf("%d! = %d\n", number, result);
}

int fact_cal(int number){
	/***********************************************************
	 This function calculate factorial of parameter 'int number'.
	 The method of calculation is recursion. 
	 ***********************************************************/
	if(number <= 1)return 1;
	else return  number * fact_cal(number - 1);
}
