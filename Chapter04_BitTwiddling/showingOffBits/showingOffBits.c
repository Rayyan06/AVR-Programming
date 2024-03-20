// ------------ Preamble ------------ // 
#include <avr/io.h>
#include <util/delay.h>

// ------------ Variables ------------ // 
#define DELAYTIME           85          /* milliseconds */
#define LED_PORT            PORTD       /* LED ports */
#define LED_DDR             DDRD        /* LED data register */
#define STARTING_LED        2           /* Start at the second LED */
#define LED_COUNT           6           /* Number of LED's */
// The final LED, calculated with offset
#define FINAL_LED           STARTING_LED + LED_COUNT


int main(void) {

    uint8_t i; 
    uint8_t repetitions;                /* Random Repetitions */
    uint8_t whichLED;
    uint16_t randomNumber = 0x1234;

    // ------------ Inits ------------ // 
    LED_DDR = 0xFF;                     /* Set up LED pins for output **/
    
    // ------------ Event Loop ------------ //    
    while (1) {
        /* Going Left */
        for (i = STARTING_LED; i < FINAL_LED; i++) {
            LED_PORT |= (1 << i);       /* Turn on the i'th pin */
            _delay_ms(DELAYTIME);       /* wait */
        }
        for (i = STARTING_LED; i < FINAL_LED; i++) {
            LED_PORT &= ~(1 << i);      /* Turn off the i'th pin */
            _delay_ms(DELAYTIME);       /* wait */
        }
        _delay_ms(5 * DELAYTIME);

        /* Going Right */
        for (i = FINAL_LED - 1; i < 255; i--) {
            LED_PORT |= (1 << i);       /* turn on the i'th pin */
            _delay_ms(DELAYTIME);       /* wait */
        }
        _delay_ms(DELAYTIME * 5);  /* pause */

        /* Toggle "random" bits for a while */
        for (repetitions = 0; repetitions < 75; repetitions++) {
            /* "random" number generator */
            randomNumber = 2938 * randomNumber + 19872;
            /* low three bits from high byte */
            whichLED = (randomNumber >> 8) & 0b00000011;

            LED_PORT ^= (1 << (whichLED + STARTING_LED));  /* Toggle our random LED */
            _delay_ms(DELAYTIME);         /* wait */
        }
        LED_PORT = 0x00;                  /* All LED's off */
        _delay_ms(DELAYTIME * 5);         /* pause */

    }
    return (0);
}