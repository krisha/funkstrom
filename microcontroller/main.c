#define F_CPU (8000000L)
#define BAUD 57600L
#define UBRR0 ((F_CPU+BAUD*8)/(BAUD*16)-1)

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>

#define VAL_VSS   0b10001000
#define VAL_VCC   0b11101110
#define VAL_FLW   0b10001110
#define VAL_STP   0x80

enum states
{
	START1 = 0,
	START2,
	DATA,
	STOP2,
	IGNORE
};

volatile uint8_t buffer[100];
volatile enum states state;
volatile uint8_t current_rx;
volatile uint8_t toggle;

volatile uint8_t current_bit;
volatile uint8_t current_byte;

static inline void led(uint8_t on)
{
	PORTD = on << PD5;
}

static inline void send(uint8_t on)
{
	PORTB = on << PB0;
}

static inline void serial_send(uint8_t c)
{
	while (!(UCSRA & (1 << UDRE)));
	UDR = c;

}

ISR(TIMER1_COMPA_vect)
{
	uint8_t on = 0;

	if ( state != IGNORE )
		return;

	on = (buffer[current_byte] >> (7-current_bit)) & 1;

	send(on);

	current_bit++;
	if ( current_bit == 8 )
	{
		current_bit = 0;
		if ( current_byte < current_rx )
			current_byte++;
		else
		{
			/* end now */
			current_byte = 0;
			state = START1;

			/* ACK */
			serial_send(0x12);
			led(0);
		}
	}
}

ISR(USART_RX_vect)
{
	uint8_t tmp = UDR;

	switch ( state )
	{
		case START1:
			//PORTD = 0;
			if ( tmp == 0xAA )
				state = START2;
			break;
		case START2:
			if ( tmp == 0x55 )
			{
				state = DATA;
				current_rx = 0;
			}
			else
				state = START1;
			break;
		case DATA:
			if ( tmp == 0x55 )
				state = STOP2;
			else
				buffer[current_rx++] = tmp;
			break;
		case STOP2:
			if ( tmp == 0xAA )
			{
				/* send now */
				//PORTD = 1 << PD5;
				state = IGNORE;
				led(1);
			}
			else
			{
				state = DATA;
				buffer[current_rx++] = 0x55;
				buffer[current_rx++] = tmp;
			}
			break;
		case IGNORE:
			/* state will be left after timer sent out all data */
			break;
	}
}

void test_send ()
{
	char data[] = { VAL_VSS, VAL_VSS, VAL_VSS, VAL_VSS, VAL_VSS,
			VAL_VSS, VAL_FLW, VAL_FLW, VAL_FLW, VAL_FLW,
			VAL_VSS, VAL_FLW,
			VAL_STP  };

	memcpy ( (uint8_t*)buffer, data, sizeof ( data ) );
	current_rx = sizeof ( data );
	state = IGNORE;
}

int main ()
{
	/* setup pins */
	DDRD = 1 << PD5;	/* led */
	DDRB = 1 << PB0;	/* send pin */

	/* setup uart */
	cli();

	UBRRH = (uint8_t)(UBRR0 >> 8);
	UBRRL = (uint8_t)UBRR0;

	UCSRB = (1 << RXEN) | (1 << TXEN) | (1 << RXCIE);

	/* setup timer irq: 330 - 331 us */
	TCCR1B = (1 << CS11) | (1 << WGM12);
	OCR1A = 334;  //measured output for 330 -> 326us. faq-

	TIMSK |= 1 << OCIE1A;

	sei();

	while ( 1 )
	{
		//PORTD ^= 1 << PD5;
		//_delay_ms ( 2500 );
		//test_send();
		//_delay_ms ( 20 );
		//test_send();
		//_delay_ms ( 20 );
		//test_send();
	}
}

