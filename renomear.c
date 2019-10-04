#include <stdio.h>
#include "SE.h"
#include <unistd.h>
#include <stdlib.h>

int main(){

	while(1){//laço para manter o codigo rodando
		leds(0);//função para apagar todos os leds
		usleep(100000);//interromper o processador pelo tempo especificado entre parenteses
		leds(255);//função para acender todos os leds
		usleep(100000);//interromper o processador pelo tempo especificado entre parenteses
		}

return 0;
}
