/*
 * Catch_The_Light.c
 *
 * Created: 10/18/2019 9:08:03 PM
 * Author : pimen
 */ 

void Cycle_lights();
void All_On();
void Cycle_lights_Rev();

#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/interrupt.h>

//uint16_t Ledtab [] = {0x08, 0x04, 0x02, 0x01, 0x20, 0x16, 0x08, 0x04, 0x16, 0x08, 0x04, 0x02, 0x01, 0x80, 0x40, 0x20};
uint8_t Ledtab [] = {3, 2, 1, 0, 5, 4, 3, 2, 4, 3, 2, 1, 0, 7, 6, 5};

int Flashreg = 0;
int Currentport = 1;

int main(void)
{
	DDRD &= 0xFF;
	DDRB &= 0xFF;
	DDRC &= 0xFF;
	
	
	PORTB &= 0;
	PORTD &= 0; //Turn off leds
	
	//All_On();
	
    /* Replace with your application code */
    while (1) 
    {
		Cycle_lights();
    }
}

void Cycle_lights()
{
	PORTD &= 0x0; //turn off all led
	PORTB &= 0x0;
	PORTC &= 0x0;
	//int Pin = Ledtab[Flashreg];
	if(Flashreg < 4 || Flashreg > 12)
	{
		//PORTD = Ledtab[Flashreg];
		PORTD |= (1<< Ledtab[Flashreg]);
	}
	else if(Flashreg < 8)
	{
		//PORTC = Ledtab[Flashreg];
		PORTC |= (1<< Ledtab[Flashreg]);
	}
	else
	{
		//PORTD = Ledtab[Flashreg];
		PORTB |= (1<< Ledtab[Flashreg]);
	}
	
	Flashreg = (Flashreg +1) & 15;
	
	_delay_ms(50);
	
	/*
	if(Currentport)
	{
		PORTD |= (1<< Flashreg);
	}
	else
	{
		PORTB |= (1<< Flashreg);
	}
	
	Flashreg = (Flashreg+1) & 7; //once Flashreg rolls over to 8, rest to 0
	if (Flashreg == 0) //when Flashreg is 0 again, Next port of leds is to be flashed
	{
		Currentport = Currentport^1; //toggle Currentport between 1 and 0
	}	
	_delay_ms(50);*/
}

void Cycle_lights_Rev()
{
	PORTD &= 0x0; //turn off all led
	PORTB &= 0x0;
	
	if(Currentport)
	{
		PORTB |= (1<< Flashreg);
	}
	else
	{
		PORTD |= (1<< Flashreg);
	}
	
	Flashreg = Flashreg-1; //once Flashreg rolls over to 0, 
	if (Flashreg == 0) //when Flashreg is 0 again, Next port of leds is to be flashed
	{
		Currentport = Currentport^1; //toggle Currentport between 1 and 0
		Flashreg = 7; // reset
	}
	_delay_ms(50);
}

void All_On ()
{
	int Flashtime = 0;
	while (Flashtime < 10)
	{
		PORTB &= 0x0; //all off but PB7 the target led
		PORTD &= 0x0;
		_delay_ms(100);
		
		PORTB |= 0xFF; //all on
		PORTD |= 0xFF;
		_delay_ms(100);
		
		Flashtime++;
	}
	
}