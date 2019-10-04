#ifndef SE_INCLUDED
#define SE_INCLUDED

unsigned char buttons(void);
int button(int botao);
int poti2c(int channel);
void i2cAD();
void led(int led, int comando);
void leds(unsigned char led);
void buzzer(int estado);
//void mapSemaforo( int semaf, char cor);
//void ledSemaforo(int led,int comando);
void iniciarSemaforo(int modo);
//void rgb(void);
void fmotor(int motor, int dir, int pwm);
void fservo(int servo, int angulo);
//int SpiOpenPort (int spi_device);
//int SpiClosePort (int spi_device);
//int SpiWriteAndRead (int spi_device, unsigned char *data, int length);
//int potenciometro(int channel); //Ler potenciometro
void LCD_init(void); //Funcao para inicializar o LCD
void LCD_write(unsigned char str[80]); //Escrever string no LCD
void comandoLCD(unsigned char* comandos);
unsigned char LCD_read(void);
void dig1(int en, int number, int dp);
void dig2(int en, int number, int dp);
void dig3(int en, int number, int dp);
void dig4(int en, int number, int dp);
void set_timer();
int get_timer(long int usec);
#endif
