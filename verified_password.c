#include <stdio.h>
#include "SE.h"
#include <string.h>

int main(){

char real_password[] = "6666";
char user_password[10];

while(1){
	printf("Digite a senha de sua conta bancaria\n");
	scanf("%s", &user_password);
	if(strcmp(real_password, user_password) == 0){
		printf("Pegue seu dinheiro\n");
		break;
	}
	else
		printf("Error: Senha Incorreta. Tente novamente\n");
}
return 0;
}
