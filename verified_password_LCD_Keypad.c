#include <stdio.h>
#include "SE.h"
#include <string.h>

int main(){

char real_password[] = "6666";
char user_password[10];
unsigned char erase[] = {12, ';'};
LCD_init();
while(1){
	LCD_write("Digite sua senha\n");
	scanf("%s", &user_password);
	if(strcmp(real_password, user_password) == 0){
		LCD_write("Pegue seu dinheiro\n");
		break;
	}
	else{
		LCD_write("Error: Senha Incorreta\n");
		LCD_write("Tente novamente\n");
		//comandoLCD(erase);
	}
}
return 0;
}

