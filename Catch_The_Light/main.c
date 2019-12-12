/*
 * Catch_The_Light.c
 *
 * Created: 10/18/2019 9:08:03 PM
 * Author : pimen
 */ 

void Cycle_lights();
void All_On(int Flashtime);
void FlashPort(int Led);
void Cycle_lights_Rev();
void mydelay(volatile int Delay);
void Light_Direction();
int Delay_Times(int win_num);
void Did_you_catch_the_light();

#include <avr/io.h>
#define F_CPU 1000000UL
#include <util/delay.h>
#include <avr/interrupt.h>

//uint16_t Ledtab [] = {0x08, 0x04, 0x02, 0x01, 0x20, 0x16, 0x08, 0x04, 0x16, 0x08, 0x04, 0x02, 0x01, 0x80, 0x40, 0x20};
//PORTD has Leds 1-4, 14-16
//PORTB has Leds 9-13
//PORTC has Leds 5-8
//-------------Led # 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16
uint8_t Ledtab [] = {3, 2, 1, 0, 5, 4, 3, 2, 4, 3, 2, 1, 0, 7, 6, 5};


int Flashreg = 0;
#define button 1
int Direction = 1;
volatile int Delay_Time = 390; //100ms with 256 pre
int Win_time = 0;

int main(void)
{
	DDRD |= 0xFF;
	DDRB |= 0x7F;
	DDRC = 0xFF & ~(1<<button) ;
	
	
	PORTC |= (1<<button); //enable pull-up resistor for PB7
	
	PORTD |= 0x0; //Turn off leds
	PORTB |= ~(0x7F);
	
	
	//PORTC |= 0x0;
	TCCR1A = (0<<COM1A1)|(0<<COM1B1)|(0<<COM1A0)|(0<<COM1B0)|(0<<WGM11)|(0<<WGM10);//0x2; // This is in CTC Mode
	TCCR1B = (0<<ICNC1)|(0<<ICES1)|(0<<WGM13)|(1<<WGM12)|(1<<CS12)|(0<<CS11)|(0<<CS10); //256 pre)
	TCCR1C = (0<<FOC1A)|(0<<FOC1B); //disable force output compare
	OCR1A = Delay_Time;
	TCNT1 = 0;
	TIMSK1 = (0<<ICIE1)|(0<<OCIE1B)|(1<<OCIE1A)|(0<<TOIE1);
	
	PCICR |= (1 << PCIE1);
	PCMSK1 |= (1 << PCINT9);
	sei();
	
	
	All_On(5);
	

    while (1) 
    {
		//Light_Direction();
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
	
	//mydelay(Delay_Time); //delay 50ms
	
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
	
	
	//mydelay(Delay_Time); //delay 100ms
	
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

void mydelay(volatile int Delay)
{
	/*for(int i = 0; i < Delay_Time; i++)
	{
		_delay_ms(10);
	}*/
	//TCNT1 = Delay;
	//OCR1A = Delay;
}

ISR(PCINT1_vect)
{
	if( (PINC & (1<<button)) == 0 ) //button goes low
	{
		Did_you_catch_the_light();
	}
}

void Did_you_catch_the_light()
{
	cli();
	if(Flashreg == 0)
	{
		All_On(5);
		Direction ^= 1;
		if(Win_time > 1)
		{
			Delay_Time = Delay_Times(Win_time);//-= 78; //subtract 20ms
			//OCR1A = Delay_Times(Win_time); //Delay_Time;
			if(Delay_Time < 1)
			{
				Win_time = 0;
				Delay_Time = Delay_Times(Win_time);//390; //reset to 100ms
				//OCR1A = Delay_Times(Win_time);//Delay_Time;
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
		Win_time = 0;
		Delay_Time = Delay_Times(Win_time);//390; //100 ms
		
		//OCR1A = Delay_Times(Win_time);
	}
	sei();
}

int Delay_Times(int win_num)
{
	int delay = 0;
	switch(win_num)
	{
		case 0:
		case 1:
			delay = 390;
			break;
		case 2:
			delay = 312;
			break;
		case 3:
			delay = 234;
			break;
		case 4:
			delay = 156;
			break;
		case 5:
			delay = 78;
			break;
	}
	return delay;
}

ISR(TIMER1_COMPA_vect)
{
	TIFR1 = (1<<OCF1A);
	OCR1A = Delay_Time;
	Light_Direction();
}
