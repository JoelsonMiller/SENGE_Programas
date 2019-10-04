#include <stdio.h>
#include "SE.h"

int main(){
int counter = 0;
int i = 1;
	while(1){
		counter = 0;
		for(i = 1; i <= 8; i++){
			if(button(i)){
				counter++;
			}
		}
		if(counter!=0){
			leds(0x00);
			usleep(1000000/counter);
			leds(0xFF);
			usleep(1000000/counter);
		}
		else
			leds(0x00);
	}

return 0;
}
