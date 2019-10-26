/*
 * Catch_The_Light.c
 *
 * Created: 10/18/2019 9:08:03 PM
 * Author : pimen
 */ 

void Cycle_lights();
void All_On(int Flashtime);
void FlashPort();
void Cycle_lights_Rev();
void Random_Light();

#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/interrupt.h>

//uint16_t Ledtab [] = {0x08, 0x04, 0x02, 0x01, 0x20, 0x16, 0x08, 0x04, 0x16, 0x08, 0x04, 0x02, 0x01, 0x80, 0x40, 0x20};
uint8_t Ledtab [] = {3, 2, 1, 0, 5, 4, 3, 2, 4, 3, 2, 1, 0, 7, 6, 5};

int Flashreg = 0;
int Num = 15;

int main(void)
{
	DDRD &= 0xFF;
	DDRB &= 0xFF;
	DDRC &= 0xFF;
	
	
	PORTB &= 0;
	PORTD &= 0; //Turn off leds
	
	All_On(10);
	int temp = 0;
    /* Replace with your application code */
    while (1) 
    {
		
		while(temp < 16)
		{
			Cycle_lights();	
			temp++;
		}
		
		temp = 0;
		
		while(temp < 16)
		{
			Cycle_lights_Rev();
			temp++;
		}
		temp = 0;
		
		All_On(10);
    }
}

void FlashPort()
{
	if(Flashreg < 4 || Flashreg > 12)
	{
		PORTD |= (1<< Ledtab[Flashreg]);
	}
	else if(Flashreg < 8)
	{
		PORTC |= (1<< Ledtab[Flashreg]);
	}
	else
	{
		PORTB |= (1<< Ledtab[Flashreg]);
	}
}

void Cycle_lights()
{
	PORTD &= 0x0; //turn off all led
	PORTB &= 0x0;
	PORTC &= 0x0;
	
	FlashPort();
	
	Flashreg = (Flashreg +1) & 15;
	
	_delay_ms(100);
	
}

void Cycle_lights_Rev()
{
	PORTD &= 0x0; //turn off all led
	PORTB &= 0x0;
	PORTC &= 0x0;
	
	FlashPort();
	
	Flashreg --;
	
	if (Flashreg < 0)
	{
		Flashreg = 15;
	}
	
	
	_delay_ms(100);
	
}

void All_On (int Flashtime)
{
	int Count = 0;
	while (Count < Flashtime)
	{
		PORTB &= 0x0; 
		PORTD &= 0x08; //all off but PD3 the target led
		PORTC &= 0x0;
		_delay_ms(100);
		
		PORTB |= 0x1F; //all on
		PORTD |= 0xEF;
		PORTC |= 0x3C;
		_delay_ms(100);
		
		Count++;
	}
	
}

void Random_Light()
{
	
}