/*

Simple program to drive a 7 segment display using a SN74HC595 shift register



*/

// ----------- Preamble ----------- //
#include <avr/io.h>
#include <util/delay.h>

// ----------- #defines ----------- //
#define DATA_PIN PB3
#define LATCH_PIN PB2
#define CLOCK_PIN PB1
#define RESET_PIN PB0

#define DISPLAY_DDR DDRB
#define DISPLAY_PORT PORTB

// ----------- Global Variables ----------- //

const uint8_t digits[10] = {
    0b01111110, // 0
    0b00110000, // 1
    0b01101101, // 2
    0b01111001, // 3
    0b00110011, // 4
    0b01011011, // 5
    0b01011111, // 6
    0b01110000, // 7
    0b01111111, // 8
    0b01111011, // 9
};
// ----------- Functions ----------- //

void displayDigit(uint8_t digit)
{
    DISPLAY_PORT &= ~(1 << LATCH_PIN); // Set latch off
    for (uint8_t i = 0; i < 8; i++)
    {
        DISPLAY_PORT &= ~(1 << CLOCK_PIN);         // Write the clock low
        uint8_t bit = (digits[digit] >> i) & 0x01; // Grab the bit at the i'th position
        DISPLAY_PORT =
            bit ? (DISPLAY_PORT | (1 << DATA_PIN))
                : (DISPLAY_PORT & ~(1 << DATA_PIN)); // Write the Data pin depending on the bit

        DISPLAY_PORT |= (1 << CLOCK_PIN); // Set the clock pin high, storing the data.
    }
    DISPLAY_PORT |= (1 << LATCH_PIN); // Set latch pin on, sending the data over
}
int main(void)
{
    // ----------- Inits ----------- //
    DISPLAY_DDR |= (1 << CLOCK_PIN) | (1 << DATA_PIN) | (1 << LATCH_PIN) |
                   (1 << RESET_PIN); // Set display DDR to output

    DISPLAY_PORT |= (1 << RESET_PIN);
    // ----------- Event Loop ----------- //

    while (1)
    {
        for (uint8_t i = 0; i < 10; i++)
        {
            displayDigit(i);
            _delay_ms(250);
        }
    }
    return (0);
}