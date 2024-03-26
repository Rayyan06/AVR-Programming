/* 

    Demo of simple on/off button code
    Button connected to PD2
    LEDS's connected to PB0..PB5

*/

// ----------- Preamble ----------- //
#include <avr/io.h>
#include <util/delay.h>
#include "pinDefines.h"

#define DEBOUNCE_TIME   500 /* Microseconds */


uint8_t debounce(void) {
    if(bit_is_clear(BUTTON_PIN, BUTTON)) {
        _delay_us(DEBOUNCE_TIME);
        if(bit_is_clear(BUTTON_PIN, BUTTON)) {
            return (1);
        }
    }
        return (0);
}
int main(void)
{
    // ----------- Inits ----------- //
    uint8_t buttonWasPressed = 0;                                   /* state */
    BUTTON_PORT |= (1 << BUTTON);     /* enable the pullup on the button */
    LED_DDR = 0xFF;                      /* set up LED for output */
    int currentLED = 6;             /* Current LED being flashed on*/
    // ----------- Event Loop ----------- //

    while(1) {    

                                            /* Check if the switch button is pressed */
        if(debounce()) {   // Alternatively: ((1 << BUTTON_PIN) & PIND) == 0b00000000

            if (buttonWasPressed == 0) {
                currentLED++;                /* Shift the current LED left */

                LED_PORT = (1 << (currentLED - 1));               // Toggle the LEDS

                if (currentLED > 6) {
                    currentLED = 0;
                }
                buttonWasPressed = 1;
            }
        } 
        else {                                              /* Not Pressed */                  
            buttonWasPressed = 0; 
        }
        
        

    }
    return (0);
}