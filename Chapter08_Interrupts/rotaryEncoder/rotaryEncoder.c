/*

Basic Rotary Encoder Read

Read data and plot on serial

*/

// ----------- Preamble ----------- //
#include <avr/io.h>
#include <util/delay.h>

// ----------- #defines ----------- //
#define ENCODER_A_PIN PB2 /* A pin */
#define ENCODER_B_PIN PB3 /* B pin */

#define LOG_FREQ 10 /* number of times a second to log encoder data */

// ----------- Global Variables ----------- //
volatile uint8_t encoderPosition;
volatile uint8_t lastEncoderPosition;
// ----------- Functions ----------- //
static inline void initEncoderInterrupts()
{

    EIMSK |= (1 << INT0) | (1 << INT1); /* enable INT0 & INT1 */
    EICRA |= (1 << ISC00);              /* trigger when button changes */
}

ISR(INT0_vect) /* on change of encoder pin A */
{
    int encoderAPinState = PINB & (1 << ENCODER_A_PIN);
    int encoderBPinState = PINB
}

ISR(INT1_vect)
{
}
int main(void)
{
    // ----------- Inits ----------- //
    DDRB &= ~(1 << ENCODER_A_PIN) | (1 << ENCODER_B_PIN);  /* Set pins A & B as inputs */
    PORTB |= (1 << ENCODER_A_PIN) | (1 << ENCODER_B_PIN);  /* enable pull-up resistors */
    PCMSK1 |= (1 << ENCODER_A_PIN) | (1 << ENCODER_B_PIN); /* enable */
    sei();                                                 /* enable global interrupts */

    // ----------- Event Loop ----------- //

    while (1)
    {

        _delay_ms(1000 / LOG_FREQ);
    }
    return (0);
}