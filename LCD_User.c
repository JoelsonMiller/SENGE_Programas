#include <stdio.h>
#include "SE.h"

// Para limpar o LCD escrever no LCD_comando a variavel 
// unsigned char apagar[] = {12, ';'}

int main(){
	char frase[80];
	printf("Digite o valor da string\n ");
	scanf("%[^\n]s", &frase);
	//setbuf(stdin, NULL);
	LCD_init();
	LCD_write(frase);
}
