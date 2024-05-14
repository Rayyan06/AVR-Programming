/*

Template Program

the basic layout of an AVR program

*/

// ----------- Preamble ----------- //
#include "USART.h"
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

const char myVeryLongString[] PROGMEM = "\r\nHi there, this is an example of a very long string. \
\r\nThe kind you wouldn't want to store in ram.\r\n";

const uint16_t sixteenBits PROGMEM = 12345;

// ----------- #defines ----------- //

// ----------- Global Variables ----------- //

// ----------- Functions ----------- //

int main(void)
{
    // ----------- Inits ----------- //
    initUSART();
    char oneLetter;
    uint8_t i;
    // ----------- Event Loop ----------- //

    while (1)
    {
        for (i = 0; i < sizeof(myVeryLongString); i++)
        {
            oneLetter = pgm_read_byte(&(myVeryLongString[i]));
            transmitByte(oneLetter);
            _delay_ms(100);
        }
        _delay_ms(1000);

        printWord(&sixteenBits);
        transmitByte('\r');
        transmitByte('\n');
        printWord(pgm_read_word(&sixteenBits));
    }
    return (0);
}