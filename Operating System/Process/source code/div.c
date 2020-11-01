#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


void main(int argc, char* argv[]){
	/****************************************************************
	 This function take two arguments which is converted char into
	 integer.
	 And one integer is divided by other.
	 The quotient,and remainder is displayed by printf()
	 ***************************************************************/
	int queu, remain;
	int num1, num2;
	num1 = atoi(argv[1]);
	num2 = atoi(argv[2]);

	if(num2 == 0){
		printf("zero can't be divide!, program will be terminated\n");
		exit(-1);
	}

	queu = num1/num2;
	remain = num1%num2;

	printf("%d / %d = %d\n", num1, num2, queu);
	printf("%d %% %d = %d\n", num1, num2, remain);
	
}
