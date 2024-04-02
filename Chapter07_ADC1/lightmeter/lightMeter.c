/*

LightMeter.c

Basic lightmeter reading program that reads the analog data from a Light meter
converts it to digital, and adjusts 6 Leds accordingly


*/

// ----------- Preamble ----------- //
#include "USART.h"
#include "pinDefines.h"
#include <avr/io.h>
#include <stdint.h>

#include <util/delay.h>

// ----------- #defines ----------- //
#define NUM_LEDS 6

// ----------- Functions ----------- //
static inline uint8_t
scale_adc_to_leds(uint16_t adcValue) /* Scales ADC value(0 - 1023)  to Led Value(0 to NUM_LEDS)*/
{
    /* Scaled ADC value, fitting in the range [0, NUM_LEDS - 1]*/
    uint8_t scaledValue = (adcValue * (NUM_LEDS - 1)) / 255 + 127;
    return scaledValue;
}
static inline void initADC0(void)
{
    ADMUX |= (1 << REFS0);                 /* Set reference voltage to AVCC */
    ADCSRA |= (1 << ADPS1) | (1 << ADPS0); /* set prescaler to /8 */
    ADCSRA |= (1 << ADEN);                 /* enable ADC */
}

int main(void)
{
    // ----------- Inits ----------- //
    uint8_t ledValue;
    uint16_t adcValue;
    uint8_t i;

    initADC0();
    initUSART();

    LED_DDR = 0xFF; /* enable output on LED pins */

    // ----------- Event Loop ----------- //
    while (1)
    {
        ADCSRA |= (1 << ADSC); /* ADC Start Conversion */
        do
        {
            // Wait until ADC conversion is done
        } while (bit_is_set(ADCSRA, ADSC));
        adcValue = ADC;             /* read ADC */
        ledValue = (adcValue >> 7); /* Set the appropriate LED */

        printBinaryByte(adcValue);
        printString("\r\n");
        /* Light up all LED's up to Led Value */
        LED_PORT = 0;
        for (i = 0; i <= ledValue; i++)
        {
            LED_PORT |= (1 << i);
        }

        _delay_ms(50); /* End event loop*/
    }
    return (0);
}