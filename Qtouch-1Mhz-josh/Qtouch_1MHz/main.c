#include <atmel_start.h>
#include <touch.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

void setLED(uint8_t ledIndex, uint8_t state);
uint8_t is_touched();

#define sleepTicks 29296	//30 sec

uint8_t Direction = 0;
uint8_t ledPosition = 0;
uint8_t gamePause = 0;
uint8_t delayIndex = 0;
uint8_t sleepCounter = 0;

const uint16_t LEDPorts[] = {0x0B, 0x0B, 0x0B, 0x0B, 0x08, 0x08, 0x08, 0x08, 0x05, 0x05, 0x05, 0x05, 0x05, 0x0B, 0x0B, 0x0B};
							// define  port addresses
const uint16_t LEDBits[] = {3, 2, 1, 0, 5, 4, 3, 2, 4, 3, 2, 1, 0, 7, 6, 5};
							// port bits for leds
const uint16_t DelayTimes[] = {20000, 15000, 10000, 5000, 2500, 1250, 625, 310, 150, 50};
							//counter times for different speeds
const uint8_t DelayMax = 9;	//number of things in delaytimes - 1

int main(void)
{	
	DDRD = 0xFF;
	DDRB = 0xFF;
	DDRC = 0xFF;
	
	atmel_start_init();		// init qtouch
	cpu_irq_enable();		// ---
	
	TCCR1B = (0<<WGM13)|(1<<WGM12)|(0<<CS12)|(1<<CS11)|(0<<CS10); 
	TIMSK1 = (0<<ICIE1)|(0<<OCIE1B)|(1<<OCIE1A)|(0<<TOIE1);
	OCR1A = DelayTimes[0];
	
	sei();

	for(uint8_t counter = 1; counter <= 6; counter++){ // flash all leds 4 times
		for(uint8_t led = 0; led < 16; led++){
			setLED(led, (counter%2));
		}
		_delay_ms(125);
	}

	while(1)
	{
		if(is_touched()){
			TCNT3 = 0;	//reset sleep counter
			gamePause = 1;
			
			
			if(ledPosition == 0){
				// good button press
				Direction = !Direction;			// switch direction
				delayIndex = (delayIndex+1) > DelayMax ? DelayMax : (delayIndex+1);	// increase speed up to max
				OCR1A = DelayTimes[delayIndex];		// set timer to speed
				
				for(uint8_t counter = 1; counter <= 4; counter++){ // flash all leds 4 times
					for(uint8_t led = 0; led < 16; led++){		
						setLED(led, (counter%2));
					}
					_delay_ms(125);
				}
				
			}else{
				if((Direction & (ledPosition < 8)) || (!Direction & (ledPosition > 8))){
					Direction = !Direction;
				}
				// bad button press
				delayIndex = 0;		// reset speed
				OCR1A = DelayTimes[delayIndex];
				_delay_ms(500);		// wait before starting again
			}
			
			while(is_touched())
			{
				;					// wait until un-hold
			}
			TCNT1 = 0;				// reset time 
			gamePause = 0;			// unpause game
		}
		
		for(uint8_t led = 0; led < 16; led++){		// show current led
			setLED(led, led == ledPosition);
		}
		
	}
}

void setLED(uint8_t ledIndex, uint8_t state){
	if(state){
		_SFR_IO8(LEDPorts[ledIndex]) |= 1 << LEDBits[ledIndex];		// set led to given state
	}else{
		_SFR_IO8(LEDPorts[ledIndex]) &= ~(1 << LEDBits[ledIndex]);
	}
}

uint8_t is_touched()
{
	touch_process();	//check touch button
	return (get_sensor_state(0) & 0x80) != 0;
}

ISR(TIMER1_COMPA_vect)
{
	if(!gamePause){
		if(Direction){	// increment or decrement led based on direction
			ledPosition = (ledPosition + 15) % 16;
			}else{
			ledPosition = (ledPosition + 1) % 16;
		}
	}
}