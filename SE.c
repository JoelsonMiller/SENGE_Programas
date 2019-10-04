#include <fcntl.h> //Needed for SPI port
#include<sys/ioctl.h> //Needed for SPI port
#include <linux/spi/spidev.h> //Needed for SPI port
#include <unistd.h> //Needed for SPI port
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>
#include <wiringPi.h>
#include <softPwm.h>

#define SECOND 1000000000	//Constante utilizada no temporizador


char PCF[8] = {0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27}; // Holds the address of all PCFs onboard
char LCD = 0xC6>>1;
int pcf[8], i, j, lcd;  // File descriptor for I2C on PCF, loop variable
char *I2c = "/dev/i2c-1";   // Peripheral used
unsigned char buf[80];  // Data buufer for I2C
long int difference, start;	//Variaveis para o temporizador
struct timespec gettime;	//Variavel do temporizador


/*########################################################
FUNÇÃO DE BOTÕES
#########################################################*/

unsigned char buttons(void){
    pcf[1] = open(I2c, O_RDWR);   // Performs the allocation for READ/WRITE MODES on each file descriptor
    ioctl(pcf[1], I2C_SLAVE, PCF[1]);   // Relates address on each file for each slave

    usleep(1000);
    read(pcf[1], buf, 1); // Read the push button PCF as a byte and put data on the buffer

    return buf[0];
}

int button(int botao){
        pcf[1] = open(I2c, O_RDWR);   // Performs the allocation for READ/WRITE MODES on each file descriptor
        ioctl(pcf[1], I2C_SLAVE, PCF[1]);   // Relates address on each file for each slave

        usleep(1000);

        unsigned char mascara;
        unsigned char leitura[10];

        //SELECIONAR MASCARA A PARTIR DO BOTAO
        switch(botao){
                case 1:
                        mascara = 0b00000001;
                        break;
                case 2:
                        mascara = 0b00000010;
                        break;
                case 3:
                        mascara = 0b00000100;
                        break;
                case 4:
                        mascara = 0b00001000;
                        break;
                case 5:
                        mascara = 0b00010000;
                        break;
                case 6:
                        mascara = 0b00100000;
                        break;
                case 7:
                        mascara = 0b01000000;
                        break;
                case 8:
                        mascara = 0b10000000;
                        break;

        }

        read(pcf[1], leitura, 1);	//Realiza a leitura do valor contido no PCF dos botoes

        if((int)mascara&leitura[0]) return 1;
        else return 0;

}

/*###################################
FUNÇÃO DE LEDS
###################################*/

void led(int led, int comando){
        pcf[0] = open(I2c, O_RDWR);   // Performs the allocation for READ/WRITE MODES on each file descriptor
        ioctl(pcf[0], I2C_SLAVE, PCF[0]);   // Relates address on each file for each slave

        usleep(1000);

        unsigned char mascara;
        unsigned char leitura[10];

        //SELECIONAR MASCARA A PARTIR DO LED
        switch(led){
                case 1:
                        mascara = 0b00000001;
                        break;
                case 2:
                        mascara = 0b00000010;
                        break;
                case 3:
                        mascara = 0b00000100;
                        break;
                case 4:
                        mascara = 0b00001000;
                        break;
                case 5:
                        mascara = 0b00010000;
                        break;
                case 6:
                        mascara = 0b00100000;
                        break;
                case 7:
                        mascara = 0b01000000;
                        break;
                case 8:
                        mascara = 0b10000000;
                        break;

        }

        //LEITURA DO VALOR ATUAL CONTIDO NO PCF
        read(pcf[0],leitura,1);

        //SELECIONAR ACAO APARTIR DO COMANDO
        switch(comando){
                case 1:
                        buf[0] = leitura[0]|mascara;
                        break;
                case 0:
                        buf[0] = leitura[0]&(~mascara);
                        break;
                case 2:
                        if(leitura[0]&mascara)  buf[0] = leitura[0]&(~mascara);
                        else    buf[0] = leitura[0]|mascara;
                        break;
        }
        write(pcf[0],buf,1);
}


void leds(unsigned char led){
    pcf[0] = open(I2c, O_RDWR);   // Performs the allocation for READ/WRITE MODES on each file descriptor
    ioctl(pcf[0], I2C_SLAVE, PCF[0]);   // Relates address on each file for each slave

    buf[0] = led;
    usleep(1000);
    write(pcf[0], buf, 1); // Read the push button PCF as a byte and put data on the buffer
}

/*####################
BUZZER
#####################*/

void buzzer(int estado){
    pcf[7] = open(I2c, O_RDWR);   // Performs the allocation for READ/WRITE MODES on each file descriptor
    ioctl(pcf[7], I2C_SLAVE, PCF[7]);   // Relates address on each file for each slave

	if(estado){
    		buf[0] = 255; // Put 0xFF on the data buffer
    		write(pcf[7], buf, 1); // Write 0xFF on the buzzer PCF
	}else{
    		buf[0] = 0; // Put 0xOO on the data buffer
    		write(pcf[7], buf, 1); // Write 0x00 on the buzzer PCF
	}
}

/***************************************************************************************************
	ADS1115_sample.c - 12/9/2013. Written by David Purdie as part of the openlabtools initiative
	Initiates and reads a single sample from the ADS1115 (without error handling)
****************************************************************************************************/

int poti2c(int channel){

	int ADS_address = 0x4A;	// Address of our device on the I2C bus
	int I2CFile;

	unsigned char writeBuf[3];		// Buffer to store the 3 bytes that we write to the I2C device
	unsigned char readBuf[2];		// 2 byte buffer to store the data read from the I2C device

  	short int val;				// Stores the 16 bit value of our ADC conversion

  	I2CFile = open("/dev/i2c-1", O_RDWR);		// Open the I2C device

  	ioctl(I2CFile, I2C_SLAVE, ADS_address);   // Specify the address of the I2C Slave to communicate with

  	// These three bytes are written to the ADS1115 to set the config register and start a conversion 
  	writeBuf[0] = 1;			// This sets the pointer register so that the following two bytes write to the config register
 
	if(channel)
     		writeBuf[1] = 0xD1;   	// This sets the 8 MSBs of the config register (bits 15-8) to 11010001
  	else
     		writeBuf[1] = 0xC1;
  	writeBuf[2] = 0x03;  		// This sets the 8 LSBs of the config register (bits 7-0) to 00000011

  	// Initialize the buffer used to read data from the ADS1115 to 0
  	readBuf[0]= 0;
  	readBuf[1]= 0;

  	// Write writeBuf to the ADS1115, the 3 specifies the number of bytes we are writing,
  	// this begins a single conversion
  	write(I2CFile, writeBuf, 3);

	//printf("\nparada1\n");

  	// Wait for the conversion to complete, this requires bit 15 to change from 0->1
  	while ((readBuf[0] & 0x80) == 0)	// readBuf[0] contains 8 MSBs of config register, AND with 10000000 to select bit 15
  	{
		read(I2CFile, readBuf, 2);	// Read the config register into readBuf
  	}

	//printf("parada2\n");

  	writeBuf[0] = 0;					// Set pointer register to 0 to read from the conversion register
  	write(I2CFile, writeBuf, 1);

  	read(I2CFile, readBuf, 2);		// Read the contents of the conversion register into readBuf

  	val = readBuf[0] << 8 | readBuf[1];	// Combine the two bytes of readBuf into a single 16 bit result  

  	//printf("%d \n",val);
  	close(I2CFile);

  	return 100 - ((int)val/265);

}


/*##################################
FUNÇÃO DE SEMÁFOROS
###################################*/


        //identificacao de qual pcf eh responsavel pelo led escolhido

//Funcao ledSemaforo que recebe um valor 'led' de 1 a 30 que corresponde a um led especifico da placa de
//semaforos. O parametro 'comando' liga ou desliga o led selecionado.
//Se comando = 1, entao desligue o led. Se comando = 0, acenda o led.
//O acendimento ou desligamento de um led especifico não afeta qualquer um dos demais leds
void ledSemaforo(int led,int comando){

	unsigned char leitura[10];
	unsigned char mascara;
	int i;

	//Selecao do indice que corresponde ao PCF
	if ((led>=1)&&(led<=6)) i = 3;
	if ((led>=7)&&(led<=14)) i = 4;
	if ((led>=15)&&(led<=22)) i = 5;
	if ((led>=23)&&(led<=30)) i = 6;
	
	//Selecao da mascara para garantir que a acao de ligar/desligar afete apenas o led especificado
	if ((led==7)||(led==15)||(led==23)) mascara = 0b00000001;
	if ((led==8)||(led==16)||(led==24)) mascara = 0b00000010;
	if ((led==1)||(led==9)||(led==17)||(led==25)) mascara = 0b00000100;
	if ((led==2)||(led==10)||(led==18)||(led==26)) mascara = 0b00001000;
	if ((led==3)||(led==11)||(led==19)||(led==27)) mascara = 0b00010000;
	if ((led==4)||(led==12)||(led==20)||(led==28)) mascara = 0b00100000;
	if ((led==5)||(led==13)||(led==21)||(led==29)) mascara = 0b01000000;
	if ((led==6)||(led==14)||(led==22)||(led==30)) mascara = 0b10000000;

	//Leitura do valor contido no PCF[i]
	pcf[i] = open(I2c, O_RDWR);   // Performs the allocation for READ/WRITE MODES on each file descriptor
    	ioctl(pcf[i], I2C_SLAVE, PCF[i]);   // Relates address on each file for each slave
	usleep(1000);
	
	read(pcf[i],leitura,1);
	usleep(1000);

	//Atribuicao do buffer para ser enviado ao PFC[i]
	if(comando) buf[0] = leitura[0]|mascara;	//Comparacao OR bit a bit
	else buf[0] = leitura[0]&(~mascara);		//Comparacao AND bit a bit

	//Envio do buffer de dados ao respectivo PCF

	write(pcf[i],buf,1);
	usleep(1000);
}

//Funcao que desliga todos os leds da placa de semaforos se modo = 1, e liga todos os leds se modo = 0
void iniciarSemaforo(int modo){

	int i2;
	
	//Iniciar a comunicação I2C com os PCF's da placa de semaforo
	for(i2=3;i2<=6;i2++){
    		pcf[i2] = open(I2c, O_RDWR);   // Performs the allocation for READ/WRITE MODES on each file des
		ioctl(pcf[i2], I2C_SLAVE, PCF[i2]);   // Relates address on each file for each slave
		usleep(1000);
	}

	//Selecionar o buffer de dados a ser enviado para a placa
	//O buffer é correspondente ao modo
	if(modo) buf[0] = 0b11111111;
	else buf[0] = 0b00000000;

	//Enviar para os PCF's da placa de semaforos o buffer selecionado
	for(i2=3;i2<7;i2++){
		write(pcf[i2],buf,1);
		usleep(500);
	}
}


/*##################################
FUNÇÃO MOTORES
###################################*/


void fmotor(int motor, int direc, int pwm){
	wiringPiSetup();
	int pin_motor_direc;
	int pin_motor_en;
        if(motor==1){
                pin_motor_direc=24;
                pin_motor_en=25;
        }
        if(motor==2){
                pin_motor_direc=27;
                pin_motor_en=28;
        }
        pinMode(pin_motor_direc, OUTPUT);
        pinMode(pin_motor_en, OUTPUT);
        softPwmCreate(pin_motor_en, 0, 100);
        digitalWrite(pin_motor_direc, direc);
        softPwmWrite(pin_motor_en, pwm);
}

/*##################################
FUNÇÃO SERVOS
###################################*/


void fservo(int servo, int angulo){
	wiringPiSetup();
	int pin_servo;
	if(servo==1)
		pin_servo=26;
	if(servo==2)
		pin_servo=29;
	pinMode(pin_servo, OUTPUT);

	if(softPwmCreate(pin_servo, 0, 100)!=0){
		printf("pwmCreate ERROR\n");
	}
	angulo = (angulo+54)/9; //mapeamento de 0-180 para 6-26 (valores experimentais do servo)
	softPwmWrite(pin_servo, angulo);

}

/*###################################


//Funcao para inicializar o LCD

/*###################################
FUNÇÕES LCD
###################################*/

void LCD_init(void){
	lcd = open(I2c, O_RDWR);
	ioctl(lcd, I2C_SLAVE, LCD);   // Relates address on each file for each slave

	buf[0]=0; //"no operation", necessario para iniciar a escrita
	buf[1]=19; //ligar backlight
	buf[2]=12; //limpa a tela do LCD e move o cursor para a posio inicial
	buf[3]=6; //Faz o cursor piscar
	write(lcd, buf, 4); //Envia as configuraes para o LCD
}
//Funcao para escrita no LCD (primeiro digito é perdido)
void LCD_write(unsigned char str[80]){
        lcd = open(I2c, O_RDWR);
        ioctl(lcd, I2C_SLAVE, LCD);   // Relates address on each file for each slave
        
        char str2[20] = "";
        int i = 0;
        int j = 0;
        while(1){
                if((str[i] == '\n')||(str[i] == '\0')){
                        
                        str2[j] = '\0';

                        strcpy(buf, " ");
                        strcat(buf, str2); //copia a string recebida para o vetor buf
                        int len = strlen(str2); //guarda o tamanho do vetor na variavel len
                        buf[0] = 0; //primeira posicao do vetor sera perdida na escrita
                        write(lcd, buf, len+1); // escreve "len" bytes do vetor buf (cada posicao corresponde a uma letra)
                        buf[1] = 13;
                        if(str[i] == '\n') write(lcd, buf, 2);
                        strcpy(str2,"");
                        j = 0;

                }else{
                        str2[j] = str[i]; 
                        j++;
                }
                if(str[i]=='\0') break;
                else i++;
        }

}

//comandoLCD envia uma sequencia de comandos para LCD
void comandoLCD(unsigned char* comandos){
	lcd = open(I2c, O_RDWR);
        ioctl(lcd, I2C_SLAVE, LCD);   // Relates address on each file for each slave

	buf[0] = 0;	//primeira posicao no vetor deve ser 0 para inciar a escrita
	int i;
	for(i = 0; comandos[i] != ';'; i++){
		buf[i+1] = comandos[i];
	}
	
	//Escreve o conteudo de buf no LCD
	write(lcd,buf,i+1);
}

//Leitura do Keypad
unsigned char LCD_read(void){
	lcd = open(I2c, O_RDWR);
	ioctl(lcd, I2C_SLAVE, LCD);   // Relates address on each file for each slave

	unsigned char var;	//variavel que armazena o valor teclado
	read(lcd, buf, 3); //lê os 3 primeiros registradores (command, low byte, high byte)
	//Decodificação do valor para impressão na tela
	switch (buf[1]){ //low byte (Teclas de 1-8)
		case 1:
			var='1';
			break;
		case 2:
			var='2';
			break;
		case 4:
			var='3';
			break;
		case 8:
			var='4';
			break;
		case 16:
			var='5';
			break;
		case 32:
			var='6';
			break;
		case 64:
			var='7';
			break;
		case 128:
			var='8';
			break;
		case 0:
			switch (buf[2]){ //high byte
				case 1:
					var='9';
					break;
				case 2://****
					var=' ';
					break;
				case 4:
					var='0';
					break;
				case 8://####
					var=8; //backspace
					break;
				case 0:
					var=0;
					break;
			}
	}
	return var; //Retorna o valor da tecla para escrita no LCD
}
//Apenas para uso nas funçoes dig2, dig3 e dig4
void switchDig2(int number, int dp){
for(i=0;i<8;i++){
    pcf[i] = open(I2c, O_RDWR);   // Performs the allocation for READ/WRITE MODES on each file descriptor
    ioctl(pcf[i], I2C_SLAVE, PCF[i]);   // Relates address on each file for each slave
}

    switch(number){
            case 0:
                buf[0] = 31;
                write(pcf[5], buf, 1);
                buf[0] = 252;
                write(pcf[4], buf, 1);
                if(dp)
                    buf[0] = 159;
                else
                    buf[0] = 191;
                write(pcf[3], buf, 1);
                break;
            case 1:
                buf[0] = 223;
                write(pcf[5], buf, 1);
                buf[0] = 255;
                write(pcf[4], buf, 1);
                if(dp)
                    buf[0] = 159;
                else
                    buf[0] = 191;
                write(pcf[3], buf, 1);
                break;
            case 2:
                buf[0] = 159;
                write(pcf[5], buf, 1);
                buf[0] = 252;
                write(pcf[4], buf, 1);
                if(dp)
                    buf[0] = 95;
                else
                    buf[0] = 127;
                write(pcf[3], buf, 1);
                break;
            case 3:
                buf[0] = 159;
                write(pcf[5], buf, 1);
                buf[0] = 254;
                write(pcf[4], buf, 1);
                if(dp)
                    buf[0] = 31;
                else
                    buf[0] = 63;
                write(pcf[3], buf, 1);
                break;
            case 4:
                buf[0] = 95;
                write(pcf[5], buf, 1);
                buf[0] = 255;
                write(pcf[4], buf, 1);
                if(dp)
                    buf[0] = 31;
                else
                    buf[0] = 63;
                write(pcf[3], buf, 1);
                break;
            case 5:
                buf[0] = 63;
                write(pcf[5], buf, 1);
                buf[0] = 254;
                write(pcf[4], buf, 1);
                if(dp)
                    buf[0] = 31;
                else
                    buf[0] = 63;
                write(pcf[3], buf, 1);
                break;
            case 6:
                buf[0] = 63;
                write(pcf[5], buf, 1);
                buf[0] = 252;
                write(pcf[4], buf, 1);
                if(dp)
                    buf[0] = 31;
                else
                    buf[0] = 63;
                write(pcf[3], buf, 1);
                break;
            case 7:
                buf[0] = 159;
                write(pcf[5], buf, 1);
                buf[0] = 255;
                write(pcf[4], buf, 1);
                if(dp)
                    buf[0] = 159;
                else
                    buf[0] = 191;
                write(pcf[3], buf, 1);
                break;
            case 8:
                buf[0] = 31;
                write(pcf[5], buf, 1);
                buf[0] = 252;
                write(pcf[4], buf, 1);
                if(dp)
                    buf[0] = 31;
                else
                    buf[0] = 63;
                write(pcf[3], buf, 1);
                break;
            case 9:
                buf[0] = 31;
                write(pcf[5], buf, 1);
                buf[0] = 254;
                write(pcf[4], buf, 1);
                if(dp)
                    buf[0] = 31;
                else
                    buf[0] = 63;
                write(pcf[3], buf, 1);
                break;
            case 10:
                buf[0] = 31;
                write(pcf[5], buf, 1);
                buf[0] = 253;
                write(pcf[4], buf, 1);
                if(dp)
                    buf[0] = 31;
                else
                    buf[0] = 63;
                write(pcf[3], buf, 1);
                break;
            case 11:
                buf[0] = 127;
                write(pcf[5], buf, 1);
                buf[0] = 252;
                write(pcf[4], buf, 1);
                if(dp)
                    buf[0] = 31;
                else
                    buf[0] = 63;
                write(pcf[3], buf, 1);
                break;
            case 12:
                buf[0] = 63;
                write(pcf[5], buf, 1);
                buf[0] = 252;
                write(pcf[4], buf, 1);
                if(dp)
                    buf[0] = 223;
                else
                    buf[0] = 255;
                write(pcf[3], buf, 1);
                break;
            case 13:
                buf[0] = 223;
                write(pcf[5], buf, 1);
                buf[0] = 252;
                write(pcf[4], buf, 1);
                if(dp)
                    buf[0] = 31;
                else
                    buf[0] = 63;
                write(pcf[3], buf, 1);
                break;
            case 14:
                buf[0] = 63;
                write(pcf[5], buf, 1);
                buf[0] = 252;
                write(pcf[4], buf, 1);
                if(dp)
                    buf[0] = 95;
                else
                    buf[0] = 127;
                write(pcf[3], buf, 1);
                break;
            case 15:
                buf[0] = 63;
                write(pcf[5], buf, 1);
                buf[0] = 253;
                write(pcf[4], buf, 1);
                if(dp)
                    buf[0] = 95;
                else
                    buf[0] = 127;
                write(pcf[3], buf, 1);
                break;
            default:
                perror("Not a valid number!\n");
                break;
        }
}
void switchDig3(int number, int dp){
for(i=0;i<8;i++){
    pcf[i] = open(I2c, O_RDWR);   // Performs the allocation for READ/WRITE MODES on each file descriptor
    ioctl(pcf[i], I2C_SLAVE, PCF[i]);   // Relates address on each file for each slave
}

    switch(number){
            case 0:
                buf[0] = 233;
                write(pcf[5], buf, 1);
                if(dp)
                    buf[0] = 225;
                else
                    buf[0] = 227;
                write(pcf[3], buf, 1);
                break;
            case 1:
                buf[0] = 253;
                write(pcf[5], buf, 1);
                if(dp)
                    buf[0] = 249;
                else
                    buf[0] = 251;
                write(pcf[3], buf, 1);
                break;
            case 2:
                buf[0] = 241;
                write(pcf[5], buf, 1);
                if(dp)
                    buf[0] = 229;
                else
                    buf[0] = 231;
                write(pcf[3], buf, 1);
                break;
            case 3:
                buf[0] = 241;
                write(pcf[5], buf, 1);
                if(dp)
                    buf[0] = 241;
                else
                    buf[0] = 243;
                write(pcf[3], buf, 1);
                break;
            case 4:
                buf[0] = 229;
                write(pcf[5], buf, 1);
                if(dp)
                    buf[0] = 249;
                else
                    buf[0] = 251;
                write(pcf[3], buf, 1);
                break;
            case 5:
                buf[0] = 227;
                write(pcf[5], buf, 1);
                if(dp)
                    buf[0] = 241;
                else
                    buf[0] = 243;
                write(pcf[3], buf, 1);
                break;
            case 6:
                buf[0] = 227;
                write(pcf[5], buf, 1);
                if(dp)
                    buf[0] = 225;
                else
                    buf[0] = 227;
                write(pcf[3], buf, 1);
                break;
            case 7:
                buf[0] = 249;
                write(pcf[5], buf, 1);
                if(dp)
                    buf[0] = 249;
                else
                    buf[0] = 251;
                write(pcf[3], buf, 1);
                break;
            case 8:
                buf[0] = 225;
                write(pcf[5], buf, 1);
                if(dp)
                    buf[0] = 225;
                else
                    buf[0] = 227;
                write(pcf[3], buf, 1);
                break;
            case 9:
                buf[0] = 225;
                write(pcf[5], buf, 1);
                if(dp)
                    buf[0] = 241;
                else
                    buf[0] = 243;
                write(pcf[3], buf, 1);
                break;
            case 10:
                buf[0] = 225;
                write(pcf[5], buf, 1);
                if(dp)
                    buf[0] = 233;
                else
                    buf[0] = 235;
                write(pcf[3], buf, 1);
                break;
            case 11:
                buf[0] = 231;
                write(pcf[5], buf, 1);
                if(dp)
                    buf[0] = 225;
                else
                    buf[0] = 227;
                write(pcf[3], buf, 1);
                break;
            case 12:
                buf[0] = 235;
                write(pcf[5], buf, 1);
                if(dp)
                    buf[0] = 229;
                else
                    buf[0] = 231;
                write(pcf[3], buf, 1);
                break;
            case 13:
                buf[0] = 245;
                write(pcf[5], buf, 1);
                if(dp)
                    buf[0] = 225;
                else
                    buf[0] = 227;
                write(pcf[3], buf, 1);
                break;
            case 14:
                buf[0] = 227;
                write(pcf[5], buf, 1);
                if(dp)
                    buf[0] = 229;
                else
                    buf[0] = 231;
                write(pcf[3], buf, 1);
                break;
            case 15:
                buf[0] = 227;
                write(pcf[5], buf, 1);
                if(dp)
                    buf[0] = 237;
                else
                    buf[0] = 239;
                write(pcf[3], buf, 1);
                break;
            default:
                perror("Not a valid number!\n");
                break;
        }
}
void switchDig4(int number, int dp){
for(i=0;i<8;i++){
    pcf[i] = open(I2c, O_RDWR);   // Performs the allocation for READ/WRITE MODES on each file descriptor
    ioctl(pcf[i], I2C_SLAVE, PCF[i]);   // Relates address on each file for each slave
}

    switch(number){
            case 0:
                buf[0] = 254;
                write(pcf[5], buf, 1);
                buf[0] = 254;
                write(pcf[3], buf, 1);
                
                buf[0] = 63;
                write(pcf[4], buf, 1);
                if(dp)
                    buf[0] = 64;
                else
                    buf[0] = 80;
                write(pcf[2], buf, 1);
                break;
            case 1:
                buf[0] = 255;
                write(pcf[5], buf, 1);
                buf[0] = 255;
                write(pcf[3], buf, 1);
                
                buf[0] = 191;
                write(pcf[4], buf, 1);
                if(dp)
                    buf[0] = 192;
                else
                    buf[0] = 208;
                write(pcf[2], buf, 1);
                break;
            case 2:
                buf[0] = 255;
                write(pcf[5], buf, 1);
                buf[0] = 254;
                write(pcf[3], buf, 1);
                
                buf[0] = 63;
                write(pcf[4], buf, 1);
                if(dp)
                    buf[0] = 32;
                else
                    buf[0] = 48;
                write(pcf[2], buf, 1);
                break;
            case 3:
                buf[0] = 255;
                write(pcf[5], buf, 1);
                buf[0] = 255;
                write(pcf[3], buf, 1);
                
                buf[0] = 63;
                write(pcf[4], buf, 1);
                if(dp)
                    buf[0] = 0;
                else
                    buf[0] = 16;
                write(pcf[2], buf, 1);
                break;
            case 4:
                buf[0] = 254;
                write(pcf[5], buf, 1);
                buf[0] = 255;
                write(pcf[3], buf, 1);
                
                buf[0] = 191;
                write(pcf[4], buf, 1);
                if(dp)
                    buf[0] = 128;
                else
                    buf[0] = 144;
                write(pcf[2], buf, 1);
                break;
            case 5:
                buf[0] = 254;
                write(pcf[5], buf, 1);
                buf[0] = 255;
                write(pcf[3], buf, 1);
                
                buf[0] = 127;
                write(pcf[4], buf, 1);
                if(dp)
                    buf[0] = 0;
                else
                    buf[0] = 16;
                write(pcf[2], buf, 1);
                break;
            case 6:
                buf[0] = 254;
                write(pcf[5], buf, 1);
                buf[0] = 254;
                write(pcf[3], buf, 1);
                
                buf[0] = 127;
                write(pcf[4], buf, 1);
                if(dp)
                    buf[0] = 0;
                else
                    buf[0] = 16;
                write(pcf[2], buf, 1);
                break;
            case 7:
                buf[0] = 255;
                write(pcf[5], buf, 1);
                buf[0] = 255;
                write(pcf[3], buf, 1);
                
                buf[0] = 63;
                write(pcf[4], buf, 1);
                if(dp)
                    buf[0] = 192;
                else
                    buf[0] = 208;
                write(pcf[2], buf, 1);
                break;
            case 8:
                buf[0] = 254;
                write(pcf[5], buf, 1);
                buf[0] = 254;
                write(pcf[3], buf, 1);
                
                buf[0] = 63;
                write(pcf[4], buf, 1);
                if(dp)
                    buf[0] = 0;
                else
                    buf[0] = 16;
                write(pcf[2], buf, 1);
                break;
            case 9:
                buf[0] = 254;
                write(pcf[5], buf, 1);
                buf[0] = 255;
                write(pcf[3], buf, 1);
                
                buf[0] = 63;
                write(pcf[4], buf, 1);
                if(dp)
                    buf[0] = 0;
                else
                    buf[0] = 16;
                write(pcf[2], buf, 1);
                break;
            case 10:
                buf[0] = 254;
                write(pcf[5], buf, 1);
                buf[0] = 254;
                write(pcf[3], buf, 1);
                
                buf[0] = 63;
                write(pcf[4], buf, 1);
                if(dp)
                    buf[0] = 128;
                else
                    buf[0] = 144;
                write(pcf[2], buf, 1);
                break;
            case 11:
                buf[0] = 254;
                write(pcf[5], buf, 1);
                buf[0] = 254;
                write(pcf[3], buf, 1);
                
                buf[0] = 255;
                write(pcf[4], buf, 1);
                if(dp)
                    buf[0] = 0;
                else
                    buf[0] = 16;
                write(pcf[2], buf, 1);
                break;
            case 12:
                buf[0] = 254;
                write(pcf[5], buf, 1);
                buf[0] = 254;
                write(pcf[3], buf, 1);
                
                buf[0] = 127;
                write(pcf[4], buf, 1);
                if(dp)
                    buf[0] = 96;
                else
                    buf[0] = 112;
                write(pcf[2], buf, 1);
                break;
            case 13:
                buf[0] = 255;
                write(pcf[5], buf, 1);
                buf[0] = 254;
                write(pcf[3], buf, 1);
                
                buf[0] = 191;
                write(pcf[4], buf, 1);
                if(dp)
                    buf[0] = 0;
                else
                    buf[0] = 16;
                write(pcf[2], buf, 1);
                break;
            case 14:
                buf[0] = 254;
                write(pcf[5], buf, 1);
                buf[0] = 254;
                write(pcf[3], buf, 1);
                
                buf[0] = 127;
                write(pcf[4], buf, 1);
                if(dp)
                    buf[0] = 32;
                else
                    buf[0] = 48;
                write(pcf[2], buf, 1);
                break;
            case 15:
                buf[0] = 254;
                write(pcf[5], buf, 1);
                buf[0] = 254;
                write(pcf[3], buf, 1);
                
                buf[0] = 127;
                write(pcf[4], buf, 1);
                if(dp)
                    buf[0] = 160;
                else
                    buf[0] = 172;
                write(pcf[2], buf, 1);
                break;
            default:
                perror("Not a valid number!\n");
                break;
        }
}
void dig1(int en, int number, int dp){
for(i=0;i<8;i++){
    pcf[i] = open(I2c, O_RDWR);   // Performs the allocation for READ/WRITE MODES on each file descriptor
    ioctl(pcf[i], I2C_SLAVE, PCF[i]);   // Relates address on each file for each slave
}

    if(en){
        buf[0] = 255; // Put 0xFF on the data buffer
        for(i = 2; i < 7; i++){
            write(pcf[i], buf, 1); // Writes 1 on all pins at the PCFs for the Semaforo Board
        }
        
        switch(number){
            case 0:
                buf[0] = 244;
                write(pcf[6], buf, 1);
                if(dp)
                    buf[0] = 195;
                else
                    buf[0] = 199;
                write(pcf[4], buf, 1);
                break;
            case 1:
                buf[0] = 254;
                write(pcf[6], buf, 1);
                if(dp)
                    buf[0] = 243;
                else
                    buf[0] = 247;
                write(pcf[4], buf, 1);
                break;
            case 2:
                buf[0] = 248;
                write(pcf[6], buf, 1);
                if(dp)
                    buf[0] = 203;
                else
                    buf[0] = 207;
                write(pcf[4], buf, 1);
                break;
            case 3:
                buf[0] = 248;
                write(pcf[6], buf, 1);
                if(dp)
                    buf[0] = 227;
                else
                    buf[0] = 231;
                write(pcf[4], buf, 1);
                break;
            case 4:
                buf[0] = 242;
                write(pcf[6], buf, 1);
                if(dp)
                    buf[0] = 243;
                else
                    buf[0] = 247;
                write(pcf[4], buf, 1);
                break;
            case 5:
                buf[0] = 241;
                write(pcf[6], buf, 1);
                if(dp)
                    buf[0] = 227;
                else
                    buf[0] = 231;
                write(pcf[4], buf, 1);
                break;
            case 6:
                buf[0] = 241;
                write(pcf[6], buf, 1);
                if(dp)
                    buf[0] = 195;
                else
                    buf[0] = 199;
                write(pcf[4], buf, 1);
                break;
            case 7:
                buf[0] = 252;
                write(pcf[6], buf, 1);
                if(dp)
                    buf[0] = 243;
                else
                    buf[0] = 247;
                write(pcf[4], buf, 1);
                break;
            case 8:
                buf[0] = 240;
                write(pcf[6], buf, 1);
                if(dp)
                    buf[0] = 195;
                else
                    buf[0] = 199;
                write(pcf[4], buf, 1);
                break;
            case 9:
                buf[0] = 240;
                write(pcf[6], buf, 1);
                if(dp)
                    buf[0] = 227;
                else
                    buf[0] = 231;
                write(pcf[4], buf, 1);
                break;
            case 10:
                buf[0] = 240;
                write(pcf[6], buf, 1);
                if(dp)
                    buf[0] = 211;
                else
                    buf[0] = 215;
                write(pcf[4], buf, 1);
                break;
            case 11:
                buf[0] = 243;
                write(pcf[6], buf, 1);
                if(dp)
                    buf[0] = 195;
                else
                    buf[0] = 199;
                write(pcf[4], buf, 1);
                break;
            case 12:
                buf[0] = 245;
                write(pcf[6], buf, 1);
                if(dp)
                    buf[0] = 203;
                else
                    buf[0] = 207;
                write(pcf[4], buf, 1);
                break;
            case 13:
                buf[0] = 250;
                write(pcf[6], buf, 1);
                if(dp)
                    buf[0] = 195;
                else
                    buf[0] = 199;
                write(pcf[4], buf, 1);
                break;
            case 14:
                buf[0] = 241;
                write(pcf[6], buf, 1);
                if(dp)
                    buf[0] = 203;
                else
                    buf[0] = 207;
                write(pcf[4], buf, 1);
                break;
            case 15:
                buf[0] = 241;
                write(pcf[6], buf, 1);
                if(dp)
                    buf[0] = 219;
                else
                    buf[0] = 223;
                write(pcf[4], buf, 1);
                break;
            default:
                perror("Not a number!\n");
                break;
        }
    }
    else{
        buf[0] = 255; // Put 0xFF on the data buffer
        for(i = 2; i < 7; i++){
            write(pcf[i], buf, 1); // Writes 1 on all pins at the PCFs for the Semaforo Board
        }
        buf[0] = 15; // Put 0x0F on the data buffer
        write(pcf[6], buf, 1);
        
        switch(number){
            case 0:
                buf[0] = 244 - 240;
                write(pcf[6], buf, 1);
                if(dp)
                    buf[0] = 195;
                else
                    buf[0] = 199;
                write(pcf[4], buf, 1);
                break;
            case 1:
                buf[0] = 254 - 240;
                write(pcf[6], buf, 1);
                if(dp)
                    buf[0] = 243;
                else
                    buf[0] = 247;
                write(pcf[4], buf, 1);
                break;
            case 2:
                buf[0] = 248 - 240;
                write(pcf[6], buf, 1);
                if(dp)
                    buf[0] = 203;
                else
                    buf[0] = 207;
                write(pcf[4], buf, 1);
                break;
            case 3:
                buf[0] = 248 - 240;
                write(pcf[6], buf, 1);
                if(dp)
                    buf[0] = 227;
                else
                    buf[0] = 231;
                write(pcf[4], buf, 1);
                break;
            case 4:
                buf[0] = 242 - 240;
                write(pcf[6], buf, 1);
                if(dp)
                    buf[0] = 243;
                else
                    buf[0] = 247;
                write(pcf[4], buf, 1);
                break;
            case 5:
                buf[0] = 241 - 240;
                write(pcf[6], buf, 1);
                if(dp)
                    buf[0] = 227;
                else
                    buf[0] = 231;
                write(pcf[4], buf, 1);
                break;
            case 6:
                buf[0] = 241 - 240;
                write(pcf[6], buf, 1);
                if(dp)
                    buf[0] = 195;
                else
                    buf[0] = 199;
                write(pcf[4], buf, 1);
                break;
            case 7:
                buf[0] = 252 - 240;
                write(pcf[6], buf, 1);
                if(dp)
                    buf[0] = 243;
                else
                    buf[0] = 247;
                write(pcf[4], buf, 1);
                break;
            case 8:
                buf[0] = 240 - 240;
                write(pcf[6], buf, 1);
                if(dp)
                    buf[0] = 195;
                else
                    buf[0] = 199;
                write(pcf[4], buf, 1);
                break;
            case 9:
                buf[0] = 240 - 240;
                write(pcf[6], buf, 1);
                if(dp)
                    buf[0] = 227;
                else
                    buf[0] = 231;
                write(pcf[4], buf, 1);
                break;
            case 10:
                buf[0] = 240 - 240;
                write(pcf[6], buf, 1);
                if(dp)
                    buf[0] = 211;
                else
                    buf[0] = 215;
                write(pcf[4], buf, 1);
                break;
            case 11:
                buf[0] = 243 - 240;
                write(pcf[6], buf, 1);
                if(dp)
                    buf[0] = 195;
                else
                    buf[0] = 199;
                write(pcf[4], buf, 1);
                break;
            case 12:
                buf[0] = 245 - 240;
                write(pcf[6], buf, 1);
                if(dp)
                    buf[0] = 203;
                else
                    buf[0] = 207;
                write(pcf[4], buf, 1);
                break;
            case 13:
                buf[0] = 250 - 240;
                write(pcf[6], buf, 1);
                if(dp)
                    buf[0] = 195;
                else
                    buf[0] = 199;
                write(pcf[4], buf, 1);
                break;
            case 14:
                buf[0] = 241 - 240;
                write(pcf[6], buf, 1);
                if(dp)
                    buf[0] = 203;
                else
                    buf[0] = 207;
                write(pcf[4], buf, 1);
                break;
            case 15:
                buf[0] = 241 - 240;
                write(pcf[6], buf, 1);
                if(dp)
                    buf[0] = 219;
                else
                    buf[0] = 223;
                write(pcf[4], buf, 1);
                break;
            default:
                perror("Not a number!\n");
                break;
        }
        
    }
}
void dig2(int en, int number, int dp){
for(i=0;i<8;i++){
    pcf[i] = open(I2c, O_RDWR);   // Performs the allocation for READ/WRITE MODES on each file descriptor
    ioctl(pcf[i], I2C_SLAVE, PCF[i]);   // Relates address on each file for each slave
}

    if(en){
        buf[0] = 255; // Put 0xFF on the data buffer
        for(i = 2; i < 7; i++){
            write(pcf[i], buf, 1); // Writes 1 on all pins at the PCFs for the Semaforo Board
        }
        switchDig2(number, dp);
    }
    else{
        buf[0] = 255; // Put 0xFF on the data buffer
        for(i = 2; i < 7; i++){
            write(pcf[i], buf, 1); // Writes 1 on all pins at the PCFs for the Semaforo Board
        }
        buf[0] = 15; // Put 0x0F on the data buffer
        write(pcf[6], buf, 1);
        
        switchDig2(number, dp);
    }
}
void dig3(int en, int number, int dp){
for(i=0;i<8;i++){
    pcf[i] = open(I2c, O_RDWR);   // Performs the allocation for READ/WRITE MODES on each file descriptor
    ioctl(pcf[i], I2C_SLAVE, PCF[i]);   // Relates address on each file for each slave
}

    if(en){
        buf[0] = 255; // Put 0xFF on the data buffer
        for(i = 2; i < 7; i++){
            write(pcf[i], buf, 1); // Writes 1 on all pins at the PCFs for the Semaforo Board
        }
        
        switchDig3(number, dp);
    }
    else{
        buf[0] = 255; // Put 0xFF on the data buffer
        for(i = 2; i < 7; i++){
            write(pcf[i], buf, 1); // Writes 1 on all pins at the PCFs for the Semaforo Board
        }
        buf[0] = 15; // Put 0x0F on the data buffer
        write(pcf[6], buf, 1);
        
        switchDig3(number, dp);
    }
}
void dig4(int en, int number, int dp){
for(i=0;i<8;i++){
    pcf[i] = open(I2c, O_RDWR);   // Performs the allocation for READ/WRITE MODES on each file descriptor
    ioctl(pcf[i], I2C_SLAVE, PCF[i]);   // Relates address on each file for each slave
} 

   if(en){
        buf[0] = 255; // Put 0xFF on the data buffer
        for(i = 2; i < 7; i++){
            write(pcf[i], buf, 1); // Writes 1 on all pins at the PCFs for the Semaforo Board
        }
        
        switchDig4(number, dp);
    }
    else{
        buf[0] = 255; // Put 0xFF on the data buffer
        for(i = 2; i < 7; i++){
            write(pcf[i], buf, 1); // Writes 1 on all pins at the PCFs for the Semaforo Board
        }
        buf[0] = 15; // Put 0x0F on the data buffer
        write(pcf[6], buf, 1);
        
        switchDig4(number, dp);
    }
}

void set_timer(){
	clock_gettime(CLOCK_REALTIME, &gettime);
	start = gettime.tv_nsec;	//Guarda o tempo inicial na variavel global start
}

int get_timer(long int usec){
	long int period;

	period = 1000*usec;
	clock_gettime(CLOCK_REALTIME, &gettime);
	difference = gettime.tv_nsec - start;	//Obtem o tempo decorrido
	if(difference < 0) difference += SECOND;

	if(difference > period){
		start += period;
		if(start > SECOND) start -= SECOND;
		
		return 1;
	}

	return 0;
}
