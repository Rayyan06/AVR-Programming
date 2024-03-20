// ------------ Preamble ------------ // 
#include <avr/io.h>
#include <util/delay.h>

// ------------ Variables ------------ // 
#define DELAYTIME           85          /* milliseconds */
#define LED_PORT            PORTD       /* LED ports */
#define LED_PIN             PIND        /* LED pin bank */
#define LED_DDR             DDRD        /* LED data register */
#define STARTING_LED        2           /* Start at the second LED */
#define LED_COUNT           6           /* Number of LED's */
// The final LED, calculated with offset
#define FINAL_LED           STARTING_LED + LED_COUNT


int main(void) {
    // ------------ Inits ------------ // 
    uint8_t i = STARTING_LED; 
    LED_DDR = 0xFF;                     /* Set up LED pins for output **/

    // ------------ Event Loop ------------ //    
    while (1) {
        while(i < (FINAL_LED - 1)) {
            LED_PORT = ~(1 << i);        /* Illuminate everything except the i'th pin */
            _delay_ms(DELAYTIME);
            i++;                        /* Move to the next LED*/
        }
        while (i > STARTING_LED) {
            LED_PORT = ~(1 << i);        /* Illuminate everything except the i'th pin */
            _delay_ms(DELAYTIME);
            i--;
        }
    }
    return (0);
}