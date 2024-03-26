/*

LightMeter.c

Basic lightmeter reading program that reads the analog data from a Light meter
converts it to digital, and adjusts 6 Leds accordingly


*/

// ----------- Preamble ----------- //
#include <avr/io.h>
#include <util/delay.h>
#include "pinDefines.h"

// ----------- #defines ----------- //
#define NUM_LEDS 6

// ----------- Functions ----------- //
static inline uint8_t scale_adc_to_leds(uint16_t adcValue) /* Scales ADC value(0 - 255)  to Led Value(0 to NUM_LEDS)*/
{
    /* Scaled ADC value, fitting in the range [0, NUM_LEDS - 1]*/
    uint8_t scaledValue = (adcValue * (NUM_LEDS - 1)) / 255;
    return scaledValue;
}
static inline void initADC0(void)
{
    ADCSRA |= (1 << ADPS1) | (1 << ADPS0); /* set prescaler to 8 */
    ADCSRA |= (1 << ADEN);                 /* enable ADC */
}

int main(void)
{
    // ----------- Inits ----------- //
    uint8_t ledValue;
    uint16_t adcValue;

    initADC0();
    LED_DDR = 0xFF; /* enable output on LED pins */

    // ----------- Event Loop ----------- //
    while (1)
    {
        ADCSRA |= (1 << ADSC);
        // Or simply loop_until_bit_is_clear()
    }
    return (0);
}