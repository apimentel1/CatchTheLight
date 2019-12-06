


#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/power.h>
#include <avr/interrupt.h>

#define Delay 5000


void Enter_Sleep();
void optimize_power_consumption();

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
	
	//set_sleep_mode();
	void optimize_power_consumption();
	
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
		
		Enter_Sleep();
    } 
}


void Enter_Sleep()
{
	SMCR = (1 << SE); //sleep power save enable, enable sleep, only use before going into sleep mode
	//The MCU control register controls the placement of the interrupt vector table in order to move interrupts between application and boot space.
	//MCUCR = //not sure
	//Power reduction register
}

void optimize_power_consumption()
{
	/* Disable digital input buffer on ADC pins */
	DIDR0 = (1 << ADC5D) | (1 << ADC4D) | (1 << ADC3D) | (1 << ADC2D) | (1 << ADC1D) | (1 << ADC0D);
	DIDR0 |= 0xC0 ;   /*ADC7D and ADC6D are undefined in header file so set bits this way*/
	
	/* Disable digital input buffer on Analog comparator pins */
	DIDR1 |= (1 << AIN1D) | (1 << AIN0D);
	/* Disable Analog Comparator */
	ACSR |= (1 << ACD);
	
	
	PRR0 = (1 << PRADC)|(1<< PRUSART1) | (1<< PRUSART0)|(1<<PRTWI0); //disable ADC converter and usart 1 as it isnot used (Page 72 in data sheet)
	PRR1 = (1<<PRTWI1)|(1<<PRTIM4)|(1<<PRSPI1)|(1<<PRTIM3)|~(1<<PRPTC); //shutdown TWI1, time 4, time 3, and spi
	
	DDRD &= ~(1<<4); //pd 4 is not used
	DDRE &= ~(0x7); //PE 0,1,2 are not used
	
	PORTD |= (1<<4); //set to input
	PORTE |= 0x7; //set to input
	
	
	
	cli();
	/* Reset watchdog timer */
	wdt_reset();
	/* Clear WDRF in MCUSR */
	MCUSR &= ~(1<<WDRF);
	/* Turn off WDT */
	WDTCSR = 0x00;
	
	SMCR = (0<<SM2)|(1<<SM1)|(1<<SM0); //sleep power save enable, enable sleep, only use before going into sleep mode
}


