// POV Toy

// ----------- PREAMBLE ----------- //
#include <avr/io.h>
#include <util/delay.h>

// ----------- Variables ----------- //
// Delay between LED flashes
#define FLASH_RATE 100 

// ----------- Functions ----------- //

void LightLeds(uint8_t ledByte) 
{
    PORTD = ledByte;
    _delay_ms(FLASH_RATE);
}

int main(void)
{
    // ----------- Inits ----------- //

    // Set Data Direction Register D to Output
    DDRD = 0xff;
    // Set PortD to have one Led on
    PORTD = (1 << 2); // 0b00000100

    // ----------- Event Loop ----------- //

    while(1) {    
        LightLeds(PORTD << 1);
        LightLeds(PORTD << 1);
        LightLeds(PORTD << 1);
        LightLeds(PORTD << 1);
        LightLeds(PORTD << 1);

        LightLeds(PORTD >> 1);
        LightLeds(PORTD >> 1);
        LightLeds(PORTD >> 1);
        LightLeds(PORTD >> 1);
        LightLeds(PORTD >> 1);


        // PORTD = 0x00;
        //_delay_ms(30);

    }
    return (0);
}