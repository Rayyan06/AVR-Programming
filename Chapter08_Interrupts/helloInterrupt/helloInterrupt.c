/*

helloInterrupt.c

Demo of using interrupts for doing what they do best --
two things at once.

Flashes LED0 at a fixed rate, interrupting whenever a button is pressed.

*/

// ----------- Preamble ----------- //
#include "pinDefines.h"
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

// ----------- Functions ----------- //
ISR(INT0_vect) /* Run every time their is a change in button state, interrupting the flow of the
                  program */
{
    if (bit_is_clear(BUTTON_PIN, BUTTON))
    {
        LED_PORT |= (1 << LED1);
    }
    else
    {
        LED_PORT &= ~(1 << LED1);
    }
}

void initInterrupt0(void)
{
    EIMSK |= (1 << INT0);  /* enable INT0 */
    EICRA |= (1 << ISC00); /* trigger when button changes */
    sei();                 /* set (global) interrupt enable bit */
}
int main(void)
{
    // ----------- Inits ----------- //
    LED_DDR = 0xff; /* All LEDs active */
    BUTTON_PORT |= (1 << BUTTON);
    initInterrupt0();
    // ----------- Event Loop ----------- //

    while (1)
    {

        _delay_ms(800);
        LED_PORT ^= (1 << LED0); /* Toggle LED0 */
    }
    return (0);
}