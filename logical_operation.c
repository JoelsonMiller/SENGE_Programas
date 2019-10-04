#include <stdio.h>
#include "SE.h"

int main(){

int choose_operation;

	printf("Digite a operacao que deseja realizar:\n\tNOT: 1\n\tAND: 2\n\tOR: 3\n");
	scanf("%i", &choose_operation);
	switch(choose_operation)
	{
		case 1:
			printf("A operacao eh um NOT\n");
			break;
		case 2:
			printf("A operacao eh uma AND\n");
			break;
		case 3:
			printf("A operação eh um OR\n");
			break;
		default:
			printf("ERROR: Nenhuma operacao disponivel foi escolhida\n");
	}
	while(1){
		if(choose_operation == 1) //A operação é um NOT
			if(button(1))
				led(1,0);
			else
				led(1,1);
		else if(choose_operation == 2) // A operação é um AND
			if(button(1)&&button(2))
				led(1,1);
			else
				led(1,0);
		else if(choose_operation == 3) // A operação é um OR
			if(button(1)||button(2))
				led(1,1);
			else
				led(1,0);
	}

return 0;
}
