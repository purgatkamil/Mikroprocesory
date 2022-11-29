#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include "HD44780.h"

#include "USART.h"
#include "SPI.h"

#include "TWI.c"

int main()
{
//inicjalizacja I2C/SPI/LCD…

void SPI_Init(void)
{
	DDRB = (1<<PB7)|(1<<PB4);
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
}

void ADC_Init(void)
{
	ADMUX = (1 << REFS0);
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0) | (1 << ADATE) | (1 << ADSC) | (1 << ADIE);
}

void AutoTrigger_Init(void)
{
	ADCSRA = (1 << ADATE);
	ADCSRA |= (1 << ADSC);
}


setTime(23, 59, 50); //ustawienie czasu na 23:59:50
while(1)
{
getTime(&hr, &min, &sec);
temp = getTemp(Celsius); //druga opcja powinna byc
 //temperatura w stopniach
 //Fahrenheita
volt = getVoltage();
displayData(mode, 0, 0);
//wyswietlenie ekranu na podanych wspolrzednych,
//w zaleznosci od aktualnego trybu (zmienna mode)
mode = checkBtn();
//sprawdzenie przycisku, je¿eli by³ wcisniety, to
 //zmiana wyœwietlanego ekranu
wait100ms();
}
}




unsigned int W = 0;
unsigned int adc;



char SPI_Transmit(char cData)
{
	SPDR = cData;
	while(!(SPSR & (1<<SPIF)));
	return SPDR;
}





void adc_read()
{
	W = ADCL;
	W |= (ADCH << 8);	
	adc = (W * 5 * 10) / 1024;
}

ISR(ADC_vect)
{
	adc_read();
}

int main(void)
{
char str[20];
int A1,A2,A3;
char temperature[10];

char j;
int B1, B2, B3;

DDRD |= (1 << PD1);

LCD_Initialize();
ADC_Init();
SPI_Init();
TWI_Init();
//AutoTrigger_Init();
sei();

TWI_Start();                        // start transmission
TWI_Write(0xD0);                    // write addres of M41T00
TWI_Write(0x00);                    // select hours register                                           // write address for reading data
TWI_Write(0x50);				 // read hours register 
TWI_Write(0x59);				 // read hours register                                        // write address for reading data
TWI_Write(0x23);				 // read hours register
TWI_Stop();                         // stop transmission 

while(1)
{

	TWI_Start();                        // start transmission
	TWI_Write(0xD0);                    // write addres of M41T00
	TWI_Write(0x02);                    // select hours registers
	TWI_Start();                        // repeated start
	TWI_Write(0xD1);                    // write address for reading data
	B1 = TWI_Read(NACK);                // read hours register
	TWI_Stop();                         // stop transmission

	B2 = ((B1>>4)&7);     // convert seconds to ASCII char
	B3 = (B1&15);         // convert seconds to ASCII char
	itoa(B2, str, 20);
	LCD_GoTo(0, 0);
	LCD_WriteText(&str[0]);
	itoa(B3, str, 20);
	LCD_GoTo(1, 0);
	LCD_WriteText(&str[0]);

	TWI_Start();                        // start transmission
	TWI_Write(0xD0);                    // write addres of M41T00
	TWI_Write(0x01);                    // select hours register
	TWI_Start();                        // repeated start
	TWI_Write(0xD1);                    // write address for reading data
	B1 = TWI_Read(NACK);                // read hours register
	TWI_Stop();                         // stop transmission

	LCD_GoTo(2, 0);
	LCD_WriteText(":");

	B2 = ((B1>>4)&7);     // convert seconds to ASCII char
	B3 = (B1&15);         // convert seconds to ASCII char
	itoa(B2, str, 20);
	LCD_GoTo(3, 0);
	LCD_WriteText(&str[0]);
	itoa(B3, str, 20);
	LCD_GoTo(4, 0);
	LCD_WriteText(&str[0]);
	
	LCD_GoTo(5, 0);
	LCD_WriteText(":");
	
	TWI_Start();                        // start transmission
	TWI_Write(0xD0);                    // write addres of M41T00
	TWI_Write(0x00);                    // select hours register
	TWI_Start();                        // repeated start
	TWI_Write(0xD1);                    // write address for reading data
	B1 = TWI_Read(NACK);                // read hours register
	TWI_Stop();                         // stop transmission

	B2 = ((B1>>4)&7);     // convert seconds to ASCII char
	B3 = (B1&15);         // convert seconds to ASCII char
	itoa(B2, str, 20);
	LCD_GoTo(6, 0);
	LCD_WriteText(&str[0]);
	itoa(B3, str, 20);
	LCD_GoTo(7, 0);
	LCD_WriteText(&str[0]);
	
	
	/*PORTB &= ~(1 << PB4); // Enable CS
	A1 = SPI_Transmit(0); // read byte
	A2 = SPI_Transmit(0); // read byte
	PORTB |= (1 << PB4);  // Disable CS
	LCD_GoTo(0,0);
	LCD_WriteText("Voltage: ");
	if (adc >= 10)
	{
	itoa(adc, str, 10);
	 LCD_GoTo(9,0);
	 LCD_WriteText(&str[0]);
	 LCD_GoTo(10,0);
	 LCD_WriteText(".");
	 LCD_GoTo(11,0);
	 LCD_WriteText(&str[1]);	
	}
	else
	{	
	itoa(adc, str, 10);
	 LCD_GoTo(9,0);
	 LCD_WriteText("0");
	 LCD_GoTo(10,0);
	 LCD_WriteText(".");
	 LCD_GoTo(11,0);
	 LCD_WriteText(&str[0]);
	}
	
	A1 = A1 * 256;
	A2 = A2 - 7;
	A1 = (A1 + A2);
	A1 = A1 >> 3;
	A3 = A1; 
	A3 *= 0.625;
	
	LCD_GoTo(0, 1);
	LCD_WriteText("Temp: ");
	
	itoa(A3, temperature, 10);
	LCD_GoTo(7, 1);
	LCD_WriteText(&temperature[0]);
	LCD_GoTo(8, 1);
	LCD_WriteText(&temperature[1]);
	LCD_GoTo(9, 1);
	LCD_WriteText(".");
	LCD_GoTo(10, 1);
	LCD_WriteText(&temperature[2]); */


_delay_ms(100);
	}
return 0;
}
//-------------------------------------
// End of file
//-------------------------------------
