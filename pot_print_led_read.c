#include <stdio.h>
#include "SE.h"

int main(){
int pot_read;
int led_num;

	printf("Digite o LED que vc quer acender: \n");
	scanf("%i", &led_num);
	leds(0x00);
	led(led_num, 1);
	while(1){

		pot_read = poti2c(0);
		printf("O valor do potenciometro eh: %d\n", pot_read);
		usleep(1000000);
	}

return 0;
}
