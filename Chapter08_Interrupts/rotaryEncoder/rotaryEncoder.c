/*

Basic Rotary Encoder Read

Read encoder data and plot on serial

*/

// ----------- Preamble ----------- //
#include "USART.h"
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

// ----------- #defines ----------- //
#define ENCODER_A_PIN PD2 /* A pin */
#define ENCODER_B_PIN PD3 /* B pin */

#define SPEAKER PD6
#define SPEAKER_DDR DDRD
#define SPEAKER_PORT PORTD

#define LED_DDR DDRB
#define LED_PORT PORTB

#define LED0 PB0
#define LED1 PB1
#define LED2 PB2
#define LED3 PB3

#define LOG_FREQ 10 /* number of times a second to log encoder data */

// ----------- Global Variables ----------- //
volatile uint8_t
    encoderPosition; /* The current position of the encoder, indexed at 0 from initial power on */

// volatile uint8_t encoderAPinState = 0; /* The current State of the A Pin */
//  volatile int encoderBPinState = 1; /* the current state of the B pin */
//  ----------- Functions ----------- //
static inline void initEncoderInterrupts()
{

    EIMSK |= (1 << INT0) | (1 << INT1); /* enable INT0 & INT1 */
    EICRA |= (1 << ISC00);              /* trigger INT0 when button state changes */
    EICRA |= (1 << ISC01);              /* trigger INT1 when button state changes */

    sei(); /* enable global interrupts */
}

ISR(INT0_vect) /* on change of encoder pin A */
{
    // Set LED0 if encoderAPinState is 1
    if (bit_is_clear(DDRD, ENCODER_A_PIN))
    {
        SPEAKER_PORT |= (1 << SPEAKER); /* Speaker Port On*/

        LED_PORT |= (1 << LED0);
    }
    else
    {
        LED_PORT &= ~(1 << LED0);
    }
}

ISR(INT1_vect) /* on change of encoder pin B*/
{
    // Set LED1 if encoderBPinState is 1
    if (bit_is_clear(DDRD, ENCODER_B_PIN))
    {
        SPEAKER_PORT |= (1 << SPEAKER); /* Speaker Port On*/

        LED_PORT |= (1 << LED1);
    }
    else
    {

        LED_PORT &= ~(1 << LED1);
    }
}
int main(void)
{
    // ----------- Inits ----------- //
    SPEAKER_DDR |= (1 << SPEAKER);  /* Speaker Pin as Output*/
    SPEAKER_PORT |= (1 << SPEAKER); /* Speaker Port On*/

    LED_DDR = 0xFF;                                       /* Configure LED DDR as output */
    PORTD |= (1 << ENCODER_A_PIN) | (1 << ENCODER_B_PIN); /* enable pull-up resistors for encoder */
    initEncoderInterrupts();
    initUSART();

    // ----------- Event Loop ----------- //

    while (1)
    {

        printBinaryByte(DDRB);
        printBinaryByte(DDRD);

        printString("Hello? \r\n");
        _delay_ms(1000 / LOG_FREQ);
    }
    return (0);
}