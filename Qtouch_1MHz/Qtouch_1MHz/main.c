#include <atmel_start.h>
#include <touch.h>
#include <util/delay.h>
#include <avr/interrupt.h>

int is_touched();
void Did_you_catch_the_light();
void Cycle_lights();
void All_On(int Flashtime);
void FlashPort(int Led);
void Cycle_lights_Rev();
void mydelay(volatile int Delay);
void Light_Direction();

//PORTD has Leds 1-4, 14-16
//PORTB has Leds 9-13
//PORTC has Leds 5-8
//-------------Led # 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16
uint8_t Ledtab [] = {3, 2, 1, 0, 5, 4, 3, 2, 4, 3, 2, 1, 0, 7, 6, 5};
volatile int Flashreg = 0;
volatile int Direction = 1;
volatile int Delay_Time = 390;
int Win_time = 0;
volatile int Flag = 1;

	

int main(void)
{	
	DDRD = 0xFF;
	DDRB = 0xFF;
	DDRC = 0xFF;
	
	PORTB |= 0x0;
	PORTD |= 0x0; //Turn off leds
	PORTC |= 0x0;
	
	atmel_start_init();
	cpu_irq_enable();
	
	TCCR1A = 0x2; // This is in CTC Mode
	TCCR1B = 0x4; //256 pre
	TIMSK1 = (1<<OCIE1A);
	OCR1A = Delay_Time;
	sei();
	
	//All_On(5);
	while( 1)
	{
		while (!is_touched())
			;//PIND = 1<<5;
		//PORTD &=~(1<<5);
		
		Did_you_catch_the_light();
		
		//PORTD|=(1<<3);
		
		while (is_touched())
			;//PIND = 1<<2;
		//PORTD &= ~(1<<2);
		
		//PORTD&=(1<<3);*/
		
		
		
	}
}

int is_touched()
{
	uint8_t key_status1 = 0;
	
	touch_process();
	key_status1 = get_sensor_state(0) & 0x80;

	if ((0u != key_status1))
	{
		return 1;
	}
	return 0;
}

void Did_you_catch_the_light()
{
	Flag = 0;
	if(Flashreg == 0)
	{
		All_On(5);
		Direction ^= 1;
		if(Win_time > 1)
		{
			Delay_Time -= 78; //subtract 20ms
			if(Delay_Time < 1)
			{
				Delay_Time = 390; //reset to 100ms
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
		
		//mydelay(3906); //delay 1 second
		_delay_ms(1000);
		Delay_Time = 390; //100 ms
		Win_time = 0;
	}
	/*
	if(Flashreg == 0)
	{
		All_On(5);
		Direction ^= 1;
		if(Win_time > 1)
		{
			Delay_Time -= 2;
			if(Delay_Time < 1)
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
	}//*/
	Flag = 1;
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
		_delay_ms(100);
		//mydelay(10); //100ms delay
		
		Count++;
	}
	
}

void mydelay(volatile int Delay)
{
	/*for(int i = 0; i < Delay_Time; i++)
	{
		_delay_ms(10);
	}*/
	OCR1A = Delay;
}

ISR(TIMER1_COMPA_vect)
{
	if (Flag)
	Light_Direction();
}