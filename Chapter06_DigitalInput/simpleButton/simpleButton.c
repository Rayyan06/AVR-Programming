/* 

Demo of simple on/off button code
Button connected to PD2
LEDS's connected to PB0..PB5

*/

// ----------- Preamble ----------- //
#include <avr/io.h>
#include <util/delay.h>

// ----------- #defines ----------- //
#define SWITCH_BUTTON PD2


int main(void)
{
    // ----------- Inits ----------- //

    DDRD &= ~(1 << SWITCH_BUTTON); // double-check we're in input mode
    PORTD |= (1 << SWITCH_BUTTON); // enable SWITCH_BUTTON pull-up resistor

    DDRB = 0xFF;
    // ----------- Event Loop ----------- //

    while(1) {    
                                            /* Check if the switch button is pressed */
        if(bit_is_clear(PIND, SWITCH_BUTTON)) {   // Alternatively: ((1 << SWITCH_BUTTON) & PIND) == 0b00000000
            PORTB = 0b00111000;
        } 
        else {                                              /* Not Pressed */                  
            PORTB = 0b11001100; 
        }
    }
    return (0);
}