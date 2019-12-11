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
void Light_Direction();

//PORTD has Leds 1-4, 14-16
//PORTB has Leds 9-13
//PORTC has Leds 5-8
//----------------------Led # 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16
volatile uint8_t Ledtab [] = {3, 2, 1, 0, 5, 4, 3, 2, 4, 3, 2, 1, 0, 7, 6, 5};
volatile int Flashreg = 0;
volatile int Direction = 1;
volatile int Delay_Time = 390;
volatile int Win_time = 0;

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
	
	TCCR1A = (0<<COM1A1)|(0<<COM1B1)|(0<<COM1A0)|(0<<COM1B0)|(0<<WGM11)|(0<<WGM10);//0x2; // This is in CTC Mode
	TCCR1B = (0<<ICNC1)|(0<<ICES1)|(0<<WGM13)|(1<<WGM12)|(1<<CS12)|(0<<CS11)|(0<<CS10); //256 pre)
	TCCR1C = (0<<FOC1A)|(0<<FOC1B); //disable force output compare
	OCR1A = Delay_Time;//Delay_Time;
	TCNT1 = 0;
	TIMSK1 = (0<<ICIE1)|(0<<OCIE1B)|(1<<OCIE1A)|(0<<TOIE1);
	sei();
	
	//All_On(5);

	while( 1)
	{
		///*
		while (!is_touched())
			;//wait until touched
			
		Did_you_catch_the_light(); //check to see if you won
		
		while (is_touched()) 
			;//wait until released
		//*/
		
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
	cli();
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
		
		_delay_ms(1000);
		Delay_Time = 390; //100 ms
		Win_time = 0;
	}
	sei();
}

void Light_Direction()
{
	Flag = 0;
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
}

void All_On (int Flashtime)
{
	int Count = 0;
	Flag = 0;
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
	Flag = 1;
}

ISR(TIMER1_COMPA_vect)
{
	Light_Direction();
	OCR1A = Delay_Time;//Delay_Time;
}