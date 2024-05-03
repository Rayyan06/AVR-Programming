#include "DigitDisplay.h"

void displayNumber(uint8_t digit)
{
    DISPLAY_PORT &= ~(1 << LATCH_PIN); // Set latch off
    for (uint8_t i = 0; i < 8; i++)
    {
        DISPLAY_PORT &= ~(1 << CLOCK_PIN);               // Write the clock low
        uint8_t bit = (digits_array[digit] >> i) & 0x01; // Grab the bit at the i'th position
        //       0b00100000
        // bit = 0b00000001
        DISPLAY_PORT = (DISPLAY_PORT & ~(1 << DATA_PIN)) |
                       (bit << DATA_PIN); // Clear and set the DATA_PIN to bit.
        /* DISPLAY_PORT =
            bit ? (DISPLAY_PORT | (1 << DATA_PIN))
                : (DISPLAY_PORT & ~(1 << DATA_PIN)); */// Write the Data pin depending on the bit

        DISPLAY_PORT |=
            (1 << CLOCK_PIN); // Set the clock pin high, storing the data on the rising edge.
    }
    DISPLAY_PORT |= (1 << LATCH_PIN); // Set latch pin on, sending the data over to the display
}