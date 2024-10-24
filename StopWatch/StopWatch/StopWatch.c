#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

unsigned char Digit[6] = {0,0,0,0,0,0};

ISR(TIMER1_COMPA_vect){
	Digit[0]++; 		// INCREMENTS SECOND1
	if(Digit[1]>=5 && Digit[0]>9){ 		// IF SECONDS REACHED 59
		Digit[0] = 0; 		// RESET SECOND1
		Digit[1] = 0; 		// RESET SECOND2
		Digit[2]++; 		// INCREMENT MINUITE1
	}
	if(Digit[0]>9){ 		// IF SECOND1 REACHED 9
		Digit[0] = 0; 		// RESET SECOND1
		Digit[1]++; 		// INCREMENT SECOND2
	}
	if(Digit[2]>9 && Digit[3]>=5){ 		// IF MINUITS REACHED 59
		Digit[2] = 0; 		// RESET MINUITE1
		Digit[3] = 0; 		// RESET MINUITE2
		Digit[4]++; 		// INCREMENT HOUR1
	}
	if(Digit[2]>9){ 		// IF MINUITE1 REACHED 9
		Digit[2] = 0; 		// RESET MINUITE1
		Digit[3]++; 		// INCREMENT MINUIT2
	}
	if(Digit[4]>9){ 		// IF HOUR1 REACHED 9
		Digit[4] = 0; 		// RESET HOUR1
		Digit[5]++; 		// INCREMENT HOUR2
	}
}

ISR(INT0_vect){
	int i;
	for(i=0;i<6;i++){
		Digit[i] = 0; 	// CLEAR ALL DIGITS
	}
}

ISR(INT1_vect){
	TCCR1B &= ~(1<<CS10) & ~ (1<<CS11) & ~ (1<<CS12); 	// STOP CLOCK
}

ISR(INT2_vect){
	TCCR1B |= (1<<CS12); 	// SET THE CLOCK BACK TO clkI/O/256
}
void TIMER1_Init_CTC_MODE(void){

	TCCR1A = (1<<FOC1A);
	TCCR1B = (1<<WGM12) | (1<<CS12);
	TCNT1 = 0;
	OCR1A = 3906; 		// COMPARE VALUE
	TIMSK |= (1<<OCIE1A); 		// INTERRUPT ENABLE
	SREG |= (1<<7); 		// ACTIVATE GLOBAL INTERRUPTS
}
void INT0_init(void){
	DDRD &= ~ (1<<PD2);
	PORTD |= (1<<PD2); 		//ENABLE INTERNAL PULL UP

	MCUCR |= (1<<ISC01);		//TRIGGER INT0 WITH FALLING EDGE
	MCUCR &= ~(1<<ISC00);

	GICR |= (1<<INT0); 			//ENABLE INT0

	SREG |= (1<<7); 		//ACTIVE GLOBAL INTERRUPTS
}

void INT1_init(void){
	DDRD &= ~(1<<PD3);

	MCUCR |= (1<<ISC10) | (1<<ISC11);	 // ACTIVE RISING EDGE

	GICR |= (1<<INT1);

	SREG |= (1<<7);
}

void INT2_init(void){

	DDRB &= ~(1<<PB2);
	PORTB |= (1<<PB2);

	MCUCSR &= ~(1<<ISC2); 	// ACTIVE FALLING EDGE

	GICR |= (1<<INT2);

	SREG |= (1<<7);
}
int main (void){

	DDRC |= 0x0F;
	PORTC &= 0xF0;

	DDRA |= 0x3F;
	PORTA &= 0xC0;

	TIMER1_Init_CTC_MODE();
	INT0_init();
	INT1_init();
	INT2_init();

	while(1){
		PORTA = (PORTA & 0xC0); 		// CLEAR FIRST 4 PINS
		PORTA |= 0x01; 			// ENABLES PIN 1 (DIGIT 1)
		PORTC = (PORTC & 0xF0) | Digit[0]; 		// INSERT SECOND1
		_delay_us(2);
		PORTA = (PORTA & 0xC0);
		PORTA |= 0x02;
		PORTC = (PORTC & 0xF0) | Digit[1];
		_delay_us(2);
		PORTA = (PORTA & 0xC0);
		PORTA |= 0x04;
		PORTC = (PORTC & 0xF0) | Digit[2];
		_delay_us(2);
		PORTA = (PORTA & 0xC0);
		PORTA |= 0x08;
		PORTC = (PORTC & 0xF0) | Digit[3];
		_delay_us(2);
		PORTA = (PORTA & 0xC0);
		PORTA |= 0x10;
		PORTC = (PORTC & 0xF0) | Digit[4];
		_delay_us(2);
		PORTA = (PORTA & 0xC0);
		PORTA |= 0x20;
		PORTC = (PORTC & 0xF0) | Digit[5];
		_delay_us(2);
	}
}
