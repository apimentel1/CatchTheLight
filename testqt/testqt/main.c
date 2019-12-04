void Cycle_lights();
void All_On(int Flashtime);
void FlashPort(int Led);
void Cycle_lights_Rev();
void mydelay(int Delay_Time);
void Light_Direction();
void Check_Light();


#include <atmel_start.h>
#include "touch.h"
#include <util/delay.h>
#include <avr/interrupt.h>

//uint16_t Ledtab [] = {0x08, 0x04, 0x02, 0x01, 0x20, 0x16, 0x08, 0x04, 0x16, 0x08, 0x04, 0x02, 0x01, 0x80, 0x40, 0x20};
//has Leds 1-4, 14-16
//PORTB has Leds 9-13
//PORTC has Leds 5-8
//-------------Led # 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16
uint8_t Ledtab [] = {3, 2, 1, 0, 5, 4, 3, 2, 4, 3, 2, 1, 0, 7, 6, 5};

/*
int Flashreg = 0;
int Num = 15;
int Direction = 1;*/

int Flashreg = 0;
int Num = 15;
#define button 7
int Direction = 1;
int Delay_Time = 10;
int Win_time = 0;



int main(void)
{
	uint8_t key_status1 = 0;
	
	/* Initializes MCU, drivers and middleware */
	//DDRB |= (1<<5);
	DDRD = 0xFF;
	DDRB = 0xFF;
	DDRC = 0xFF;
		
		
	PORTB |= 0x0;
	PORTD |= 0x0; //Turn off leds
	PORTC |= 0x0;
	
	atmel_start_init();
	
	cpu_irq_enable();
	
	//touch_init();
	//int temp = 1;
	
	//Cycle_lights();
	//All_On(5);
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
			Check_Light();
		}
		else 
		{
			PORTB &=~(1<<5);	
		}
		Light_Direction();
	}
}


void Check_Light()
{
	if(Flashreg == 0)
	{
		All_On(5);
		Direction ^= 1;
		if(Win_time > 1)
		{
			Delay_Time -= 2;
			if(Delay_Time < 4)
			{
				Delay_Time = 10;
				Win_time = 0;
			}
		}
		Win_time++;
	}
	else
	{
		PORTD &= 0x0; //turn off all led
		PORTB &= 0x0;
		PORTC &= 0x0;
		
		FlashPort(Flashreg);
		
		mydelay(100);
		Delay_Time = 10;
		Win_time = 0;
	}
	
}

void Light_Direction()
{
	if (Direction)
	{
		Cycle_lights();
	}
	else
	{
		Cycle_lights_Rev();
	}
}

void FlashPort(int Led)
{
	//Flashreg is the index into array Ledtab
	//Ex Led # |1|2|3|...|15|16|
	//Ledtab # |0|1|2|...|14|15|
	if(Led < 4 || Led > 12)
	{
		PORTD |= (1<< Ledtab[Led]);
	}
	else if(Led < 8)
	{
		PORTC |= (1<< Ledtab[Led]);
	}
	else
	{
		PORTB |= (1<< Ledtab[Led]);
	}
}

void Cycle_lights()
{
	PORTD &= 0x0; //turn off all led
	PORTB &= 0x0;
	PORTC &= 0x0;
	
	FlashPort(Flashreg); //Flash the light
	
	Flashreg = (Flashreg + 1) & 15; //Increment Flashreg until 15 then reset to 0
	
	mydelay(Delay_Time); //delay 50ms
	
}

void Cycle_lights_Rev()
{
	PORTD &= 0x0; //turn off all led
	PORTB &= 0x0;
	PORTC &= 0x0;
	
	FlashPort(Flashreg); //Flash the led
	
	Flashreg--; //Decrement Flashreg
	
	if (Flashreg < 0) //when below 0
	{
		Flashreg = 15; //reset to 15
	}
	
	
	mydelay(Delay_Time); //delay 100ms
	
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
		mydelay(10); //100ms delay
		
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

/*
ISR(PCINT0_vect)
{
	if(PINB == (PINB & ~(1<<button))) //button goes low
	{
		if(Flashreg == 0)
		{
			All_On(5);
			Direction ^= 1;
			if(Win_time > 1)
			{
				Delay_Time -= 2;
				if(Delay_Time < 4)
				{
					Delay_Time = 10;
					Win_time = 0;
				}
			}
			Win_time++;
		}
		else
		{
			PORTD &= 0x0; //turn off all led
			PORTB &= 0x0;
			PORTC &= 0x0;
			
			FlashPort(Flashreg);
			
			mydelay(100);
			Delay_Time = 10;
			Win_time = 0;
		}
	}
}
*/