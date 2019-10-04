#include <stdio.h>
#include "SE.h"

int main(){

	unsigned char msg;
	unsigned char erase[] = {12, ';'};
	char msg_write[40];
	LCD_init();
	while(1){
		msg = LCD_read();
		sprintf(msg_write, "%c foi apertado", msg);
		comandoLCD(erase);
		LCD_write(msg_write);
		usleep(100000);
	}
return 0;
}
