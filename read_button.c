#include <stdio.h>
#include "SE.h"

int main(){
	while(1){
//		if(button(1))
//			printf("O botao foi apertado");

	int x = button(1);
	printf("O botao %d", x);
	//printf("Hello world\n");
	usleep(10000);
	}

return 0;
}
