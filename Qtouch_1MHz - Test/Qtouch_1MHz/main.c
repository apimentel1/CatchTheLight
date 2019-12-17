#include <atmel_start.h>
#include <touch.h>
#include <util/delay.h>
#include <avr/interrupt.h>

uint8_t is_touched();
void Did_you_catch_the_light();
void All_On(int Flashtime);
void FlashPort(int Led);
void Enter_sleep();

//PORTD has Leds 1-4, 14-16
//PORTB has Leds 9-13
//PORTC has Leds 5-8
//----------------------Led # 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16
uint8_t Ledtab [] = {3, 2, 1, 0, 5, 4, 3, 2, 4, 3, 2, 1, 0, 7, 6, 5};
uint16_t DelayTimes[] = {390, 312, 234, 156, 78};
uint8_t Cyclestosleep[] = {19, 23, 31, 49, 94};
#define MaxDelay 4 //five different delays possible, since array starts a 0 -> 5-1=4
uint8_t Delayreg = 0;
volatile int Flashreg = 0;
uint8_t Direction = 1;
uint8_t Win_time = 1;
uint8_t StopGame = 0; 
#define Pulluppin 0
#define Button 1
uint8_t Cyclecount = 0;
uint8_t sleep =0;
uint8_t is_Button = 0;

int main(void)
{	
	DDRD = 0xFF;
	DDRB = 0xFF;
	DDRC = 0xFF & ~(1<<Button) & ~(1<<Pulluppin);
	//DDRE = (1<<Testpin);
	
	//PORTE |= ~(1<<Testpin);
	PORTC |= (1<<Button) | (1<<Pulluppin);
	PORTB |= 0x0;
	PORTD |= 0x0; //Turn off leds
	//PORTC |= 0x0;
	
	atmel_start_init();
	cpu_irq_enable();
	cli();
	TCCR1A = (0<<COM1A1)|(0<<COM1B1)|(0<<COM1A0)|(0<<COM1B0)|(0<<WGM11)|(0<<WGM10);//0x2; // This is in CTC Mode
	TCCR1B = (0<<ICNC1)|(0<<ICES1)|(0<<WGM13)|(1<<WGM12)|(1<<CS12)|(0<<CS11)|(0<<CS10); //256 pre)
	TCCR1C = (0<<FOC1A)|(0<<FOC1B); //disable force output compare
	TIMSK1 = (0<<ICIE1)|(0<<OCIE1B)|(1<<OCIE1A)|(0<<TOIE1);
	OCR1A = DelayTimes[0];
	
	PCICR |= (1 << PCIE1);
	PCMSK1 |= (1 << PCINT9)|(1<<PCINT8);
	sei();
	
	All_On(5);//Flash all the lights on
	sei();
	
	while( 1)
	{
		if(is_touched() && !is_Button) //check and see if the button was touched
		{
			StopGame = 1; //stop the interrupt from going off and change the led value
			Did_you_catch_the_light(); //check to see if you won
			while (is_touched()) 
			;//wait until released
			TCNT1 = 0; //reset time
			StopGame = 0; //enable interrupt to change led
			Cyclecount = 0; //reset time count
		}
		if(!sleep)
		{
			FlashPort(Flashreg); //strobe the led	
		}
		if (Cyclecount >= Cyclestosleep[Delayreg])
		{
			Enter_sleep();
		}
	}
}

uint8_t is_touched()
{	
	touch_process();
	return (get_sensor_state(0) & 0x80) != 0;//return 1 if touched
											//return 0 if not touched
}

void Did_you_catch_the_light()
{
	if(Flashreg == 0) //is led 1 on
	{
		All_On(5); //flash all leds to show that you won
		Direction ^= 1; //change direction
		if(Win_time > 2)//if you win twice
		{
			Delayreg++; //increase the speed
			if(Delayreg > MaxDelay) //if delayreg is greater then the Maxdelay 
			{
				Delayreg = 0; //you beat the game and reset
				Win_time = 0;
			}
			OCR1A = DelayTimes[Delayreg]; //update CTC register 
		}
		Win_time++;
	}
	else
	{
		if(Direction & (Flashreg>12) || !Direction & (Flashreg<4)) //stop you from cheating!!!!!
			Direction ^=1; //change the direction if you are 3 leds before reaching led 1
		Delayreg = 0; //reset to start speed
		OCR1A = DelayTimes[0]; //update CTC register 
		_delay_ms(500); //wait
		Win_time = 1; //reset win times
	}
}

void FlashPort(int Led)
{
	PORTD &= 0x0; //turn off all led
	PORTB &= 0x0;
	PORTC &= 0x0;
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

void All_On (int Flashtime)
{
	int Count = 0;
	while (Count < Flashtime)
	{
		PORTB |= 0x1F; //all on
		PORTD |= 0xEF;
		PORTC |= 0x3C;
		_delay_ms(100);
		
		PORTB &= 0x0;
		PORTD &= 0x08; //all off but PD3 the target led
		PORTC &= 0x0;
		_delay_ms(100);
		Count++;
	}
}

ISR(TIMER1_COMPA_vect)
{
	if (!StopGame)
	{
		if (Direction)
		{
			Flashreg = (Flashreg + 1) & 15; //Increment Flashreg until 15 then reset to 0
			if (Flashreg == 0)
			{
				Cyclecount++;
			}
		}
		else
		{
			Flashreg--; //Decrement Flashreg
		
			if (Flashreg < 0) //when below 0
			{
				Flashreg = 15; //reset to 15
				Cyclecount++;
			}
		}
	}
}

ISR(PCINT1_vect)
{
	if((PINC & (1<<Button)) == 0)
	{
		StopGame = 1; //stop the interrupt from going off and change the led value
		Did_you_catch_the_light(); //check to see if you won
		while (is_touched())
		;//wait until released
		TCNT1 = 0; //reset time
		StopGame = 0; //enable interrupt to change led
		Cyclecount = 0; //reset time count
	}
	else 
	{
		_delay_ms(100); //debounce
		if((PINC & (1<<Pulluppin)) == 0)
		{
			is_Button = 1;
		}
		else if((PINC & (1<<Pulluppin)) == 1)
		{
			is_Button == 0;
		}
	}
	
}

void Enter_sleep()
{
	//add code to go to sleep
	PORTD &= 0x00; //turn off all led
	PORTB &= 0x00;
	PORTC &= 0x00;
	TIMSK1 &= ~(1<<OCIE1A);
	StopGame = 1;
	sleep = 1;
	
	if(Direction & (Flashreg>12) || !Direction & (Flashreg<4)) //stop you from cheating!!!!!
		Direction ^=1; //change the direction if you are 3 leds before reaching led 1
	
	while(!is_touched())
		;//wait until touched
	TIMSK1 |= (1<<OCIE1A);
	StopGame = 0; //enable interrupt to change led
	Cyclecount = 0; //reset time count
	sleep = 0;
}