


#include <avr/io.h
#define F_CPU 1000000UL
#include <util/delay.h>

#define Delay 5000;

//PORTD has Leds 1-4, 14-16
//PORTB has Leds 9-13
//PORTC has Leds 5-8
//-------------Led # 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16
uint8_t Ledtab [] = {3, 2, 1, 0, 5, 4, 3, 2, 4, 3, 2, 1, 0, 7, 6, 5};

int main(void)
{
	DDRD = 0xFF;
	DDRB = 0x7F;
	DDRC = 0xFF;
	
	PORTD |= 0x0; //Turn off leds
	PORTB |= 0x0;
	PORTC |= 0x0;
	
    /* Replace with your application code */
    while (1) 
    {
		PORTD |= (1<<Ledtab[0]); //green led
		_delay_ms(Delay);
		PORTD &= ~(1<<Ledtab[0]); //turn off
		
		PORTD |= (1<<Ledtab[1]); //blue led2
		_delay_ms(Delay);
		PORTD &= ~(1<<Ledtab[1]); //turn off
		
		PORTD |= (1<<Ledtab[2]); //red led3
		_delay_ms(Delay);
		PORTD &= (1<<Ledtab[2]); //led off
		
		PORTB |= 0x1F; //all on
		PORTD |= 0xEF;
		PORTC |= 0x3C;
		_delay_ms(Delay);
		PORTB &= 0x0;
		PORTD &= 0x0; //all off
		PORTC &= 0x0;
		
		_delay_ms(Delay); //allow for time to hold down pb
    } 
}


