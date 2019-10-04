#include <stdio.h>
#include "SE.h"

int main(){
int pot_one;
int pot_two;
LCD_init();
unsigned char erase[] = {12, ';'}; // Comando do registrador que limpa a tela do LCD
char msg[80];

	while(1){
		pot_one = poti2c(0);
		pot_two = poti2c(1);
		sprintf(msg, "Pot1: %d\nPot2: %d\n", pot_one, pot_two);
		comandoLCD(erase); //Apaga o LCD
		LCD_write(msg);
		usleep(500000);
		//usleep(100);
	}
}
