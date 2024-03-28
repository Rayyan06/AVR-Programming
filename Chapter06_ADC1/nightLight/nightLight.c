/*
NightLight.c

Simple NightLight Program

*/

// ----------- Preamble ----------- //
#include <avr/io.h>
#include <util/delay.h>

// ----------- #defines ----------- //
#include "pinDefines.h"

// ----------- Functions ----------- //
uint16_t readADC(uint8_t channel)
{
    ADMUX = (0xf0 & ADMUX) | channel; /* Select Channel in ADMUX */
    ADCSRA |= (1 << ADSC);            /* start ADC conversion */
    loop_until_bit_is_clear(ADCSRA, ADSC);
    return (ADC);
}

static inline void initADC(void)
{
    ADMUX |= (1 << REFS0);                 /* Set reference voltage to AVCC */
    ADCSRA |= (1 << ADPS1) | (1 << ADPS0); /* set prescaler to /8 */
    ADCSRA |= (1 << ADEN);                 /* enable ADC */
}
int main(void)
{
    // ----------- Inits ----------- //
    uint16_t lightThreshold;
    uint16_t sensorValue;

    initADC();

    LED_DDR = 0xFF; /* configure LEDs for output */
    // SPEAKER_DDR |= (1 << SPEAKER); /* speaker for output */

    // ----------- Event Loop ----------- //
    while (1)
    {

        lightThreshold = readADC(POT);
        sensorValue = readADC(LIGHT_SENSOR);

        if (sensorValue < lightThreshold)
        {
            LED_PORT = 0xff;
            // SPEAKER_PORT |= (1 << SPEAKER);
        }
        else
        {
            LED_PORT = 0x00;
            // SPEAKER_PORT &= ~(1 << SPEAKER);
        }
    }
    return (0);
}