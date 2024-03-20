// POV Toy

// ----------- PREAMBLE ----------- //
#include <avr/io.h>
#include <util/delay.h>

// ----------- Variables ----------- //
// Delay between LED flashes
#define FLASH_RATE 500 

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

    // Number of Bit Shifts
    uint8_t numShifts = 5;
    // ----------- Event Loop ----------- //

    while(1) {    
        for(int i = 0; i < numShifts; i++) {
            LightLeds(PORTD << 1);
        }
        for(int j = 0; j < numShifts; j++) {
            LightLeds(PORTD >> 1);
        }

        // PORTD = 0x00;
        //_delay_ms(30);

    }
    return (0);
}