#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include "HD44780.h"

#include "USART.h"
#include "SPI.h"

#include "TWI.c"

unsigned int W = 0;
unsigned int adc = 0;

void adc_read()
{
	W = ADCL;
	W |= (ADCH << 8);
	adc = (W * 5 * 10) / 1024;
}

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

char SPI_Transmit(char cData)
{
	SPDR = cData;
	while(!(SPSR & (1<<SPIF)));
	return SPDR;
}


ISR(ADC_vect)
{
	adc_read();
}

void setTime(int Hours, int Minutes, int Seconds)
{
	int jednosciHours = Hours % 10;
	int dziesiatkiHours =  (Hours - jednosciHours) / 10;
	int wynikHours = 0;
	dziesiatkiHours = dziesiatkiHours << 4;
	wynikHours = dziesiatkiHours + jednosciHours;
	
	int jednosciMinutes = Minutes % 10;
	int dziesiatkiMinutes =  (Minutes - jednosciMinutes) / 10;
	int wynikMinutes = 0;
	dziesiatkiMinutes = dziesiatkiMinutes << 4;
	wynikMinutes = dziesiatkiMinutes + jednosciMinutes;
	
	int jednosciSeconds = Seconds % 10;
	int dziesiatkiSeconds =  (Seconds - jednosciSeconds) / 10;
	int wynikSeconds = 0;
	dziesiatkiSeconds = dziesiatkiSeconds << 4;
	wynikSeconds = dziesiatkiSeconds + jednosciSeconds;
	
	TWI_Start();                        // start transmission
	TWI_Write(0xD0);                    // write addres of M41T00
	TWI_Write(0x00);                    // select hours register                                           // write address for reading data
	TWI_Write(wynikSeconds);			// read hours register
	TWI_Write(wynikMinutes);			// read hours register                                        // write address for reading data
	TWI_Write(wynikHours);				// read hours register
	TWI_Stop();							// stop transmission 
}

void getTime(int *hr,int *min,int *sec)
{
	TWI_Start();                        // start transmission
	TWI_Write(0xD0);                    // write addres of M41T00
	TWI_Write(0x02);                    // select hours registers
	TWI_Start();                        // repeated start
	TWI_Write(0xD1);                    // write address for reading data
	*hr = TWI_Read(NACK);                // read hours register
	TWI_Stop();                         // stop transmission
	
	TWI_Start();                        // start transmission
	TWI_Write(0xD0);                    // write addres of M41T00
	TWI_Write(0x01);                    // select hours registers
	TWI_Start();                        // repeated start
	TWI_Write(0xD1);                    // write address for reading data
	*min = TWI_Read(NACK);                // read hours register
	TWI_Stop(); 
	 
	TWI_Start();                        // start transmission
	TWI_Write(0xD0);                    // write addres of M41T00
	TWI_Write(0x00);                    // select hours registers
	TWI_Start();                        // repeated start
	TWI_Write(0xD1);                    // write address for reading data
	*sec= TWI_Read(NACK);                // read hours register
	TWI_Stop();  
	
}

enum TemperatureScales{Celsius = 0, Fahrenheit = 1};

int getTemp(int t)
{
	int A1,A2,A3;
	
	PORTB &= ~(1 << PB4); // Enable CS
	A1 = SPI_Transmit(0); // read byte
	A2 = SPI_Transmit(0); // read byte
	PORTB |= (1 << PB4);  // Disable CS
	
	A1 = A1 * 256;
	A2 = A2 - 7;
	A1 = (A1 + A2);
	A1 = A1 >> 3;
	A3 = A1;
	A3 *= 0.625;
	
	if(t == 0)
	{
		return A3;
	}
	else if(t == 1) 
	{
		A3 = ((A3 * 9) / 5 ) + 32;
		return A3;
	}
	else
	{
		A3 = 0;
		return A3;
	}
}

int getVoltage()
{
	int x = adc;
	return x;
}

int main(void)
{

	
	char str[20];
	char temperature[10];
	
int Godziny;
int Minuty;
int Sekundy;	
	
int volt = 0;

//char j;
int B1, B2, B3;

DDRD |= (1 << PD1);

LCD_Initialize();
ADC_Init();
SPI_Init();
TWI_Init();
sei();
setTime(16, 17, 18);

while(1)
{
	
	getTime(&Godziny, &Minuty, &Sekundy);
	volt = getVoltage();

/*
	B2 = ((Godziny>>4)&7);     // convert seconds to ASCII char
	B3 = (Godziny&15);         // convert seconds to ASCII char
	itoa(B2, str, 10);
	LCD_GoTo(0, 0);
	LCD_WriteText(&str[0]);
	itoa(B3, str, 20);
	LCD_GoTo(1, 0);
	LCD_WriteText(&str[0]);
	LCD_WriteText(str);

	LCD_GoTo(2, 0);
	LCD_WriteText(":");

	B2 = ((Minuty>>4)&7);     // convert seconds to ASCII char
	B3 = (Minuty&15);         // convert seconds to ASCII char
	itoa(B2, str, 20);
	LCD_GoTo(3, 0);
	LCD_WriteText(&str[0]);
	itoa(B3, str, 20);
	LCD_GoTo(4, 0);
	LCD_WriteText(&str[0]);
	
	LCD_GoTo(5, 0);
	LCD_WriteText(":");

	B2 = ((Sekundy>>4)&7);     // convert seconds to ASCII char
	B3 = (Sekundy&15);         // convert seconds to ASCII char
	itoa(B2, str, 20);
	LCD_GoTo(6, 0);
	LCD_WriteText(&str[0]);
	itoa(B3, str, 20);
	LCD_GoTo(7, 0);
	LCD_WriteText(&str[0]);*/
	
	
	LCD_GoTo(0,0);
	LCD_WriteText("Voltage: ");
	if (volt >= 10)
	{
	itoa(volt, str, 10);
	 LCD_GoTo(9,0);
	 LCD_WriteText(&str[0]);
	 LCD_GoTo(10,0);
	 LCD_WriteText(".");
	 LCD_GoTo(11,0);
	 LCD_WriteText(&str[1]);	
	}
	else
	{	
	itoa(volt, str, 10);
	 LCD_GoTo(9,0);
	 LCD_WriteText("0");
	 LCD_GoTo(10,0);
	 LCD_WriteText(".");
	 LCD_GoTo(11,0);
	 LCD_WriteText(&str[0]);
	}

int temperaturka = getTemp(Celsius);
	
	LCD_GoTo(0, 1);
	LCD_WriteText("Temp: ");
	
	itoa(temperaturka, temperature, 10);
	LCD_GoTo(7, 1);
	LCD_WriteText(&temperature[0]);
	LCD_GoTo(8, 1);
	LCD_WriteText(&temperature[1]);
	LCD_GoTo(9, 1);
	LCD_WriteText(".");
	LCD_GoTo(10, 1);
	LCD_WriteText(&temperature[2]); 


_delay_ms(100);
	}
return 0;
}
//-------------------------------------
// End of file
//-------------------------------------
