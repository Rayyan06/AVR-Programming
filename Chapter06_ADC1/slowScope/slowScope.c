/*

Slow-scope.   A free-running AVR / ADC "oscilloscope"

*/

// ----------- Preamble ----------- //
#include "USART.h"
#include <avr/io.h>
#include <util/delay.h>

// ----------- #defines ----------- //
#define SAMPLE_DELAY 20 /* ms, controls the scroll speed of the scope */

// ----------- Functions ----------- //
static inline void initFreeRunningADC(void)
{
    ADMUX |= (1 << REFS0);                 /* Set reference voltage to AVCC */
    ADCSRA |= (1 << ADPS1) | (1 << ADPS0); /* set prescaler to /8 */

    ADMUX |= (1 << ADLAR); /* left-adjust result, return only 8 bits */

    ADCSRA |= (1 << ADEN);  /* enable ADC */
    ADCSRA |= (1 << ADATE); /* enable auto-triggering ADC */
    ADCSRA |= (1 << ADSC);  /* start first conversion */
}
int main(void)
{
    // ----------- Inits ----------- //
    initUSART();
    initFreeRunningADC();
    // ----------- Event Loop ----------- //

    while (1)
    {
        transmitByte(ADCH); /* transmit the high byte, left-adjusted */
        _delay_ms(SAMPLE_DELAY);
    }

    return (0);
}