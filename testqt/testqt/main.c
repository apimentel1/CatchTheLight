void Cycle_lights();
void All_On(int Flashtime);
void FlashPort();
void Cycle_lights_Rev();
void Random_Light();
void mydelay(int Delay_Time);


#include <atmel_start.h>
#include "touch.h"
//#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/interrupt.h>

//uint16_t Ledtab [] = {0x08, 0x04, 0x02, 0x01, 0x20, 0x16, 0x08, 0x04, 0x16, 0x08, 0x04, 0x02, 0x01, 0x80, 0x40, 0x20};
//has Leds 1-4, 14-16
//PORTB has Leds 9-13
//PORTC has Leds 5-8
//-------------Led # 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16
uint8_t Ledtab [] = {3, 2, 1, 0, 5, 4, 3, 2, 4, 3, 2, 1, 0, 7, 6, 5};


int Flashreg = 0;
int Num = 15;



int main(void)
{
	uint8_t key_status1 = 0;
	
	/* Initializes MCU, drivers and middleware */
	atmel_start_init();
	
	//DDRB |= (1<<5);
	DDRD &= 0xFF;
	DDRB &= 0xFF;
	DDRC &= 0xFF;
		
		
	PORTB &= 0x0;
	PORTD &= 0x0; //Turn off leds
	PORTC &= 0x0;
	cpu_irq_enable();
	
	//touch_init();
	//int temp = 1;
	
	Cycle_lights();

	/* Replace with your application code */
	while (1)
	{
		//Run_Light(1)
		
		touch_process();

		key_status1 = get_sensor_state(0) & 0x80;

		if ((0u != key_status1) /*&& temp*/)
		{
			//Cycle_lights();
			PORTB |=(1<<5);
			//temp = 0;	
		}
		else 
		{
			PORTB &=~(1<<5);	
		}
		
	}
}

/*
void Run_Light(int true)
{
	if(true)
	{
		while(1)
		{
			Cycle_lights();
		}
	}
	else
	{
		while(1)
		{
			Cycle_lights_Rev();
		}
	}
	
}*/

void FlashPort()
{
	//Flashreg is the index into array Ledtab
	//Ex Led # |1|2|3|...|15|16|
	//Ledtab # |0|1|2|...|14|15|
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
	
	FlashPort(); //Flash the light
	
	Flashreg = (Flashreg +1) & 15; //Increment Flashreg until 15 then reset to 0
	
	mydelay(10); //delay 50ms
}

void Cycle_lights_Rev()
{
	PORTD &= 0x0; //turn off all led
	PORTB &= 0x0;
	PORTC &= 0x0;
	
	FlashPort(); //Flash the led
	
	Flashreg --; //Decrement Flashreg
	
	if (Flashreg < 0) //when below 0
	{
		Flashreg = 15; //reset to 15
	}
	
	
	mydelay(10); //delay 100ms
	
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
		mydelay(5); //50ms delay
		
		Count++;
	}
	
}

void mydelay(int Delay_Time)
{
	for(int i = 0; i < Delay_Time; i++)
	{
		_delay_ms(10);
	}
	
}

void Random_Light()
{
	
}