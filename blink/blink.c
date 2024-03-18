#include <avr/io.h>
#include <util/delay.h>

int main()
{
    // Set D8(PB0) LED pin as output
    DDRB = 0b00000001;
    while (1) {
        PORTB =  0b00000001;   // LED on
        _delay_ms(200);
        PORTB = 0b00000000;   // LED off
        _delay_ms(200);
    }
    return 0;
}