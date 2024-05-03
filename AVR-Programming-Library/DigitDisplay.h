#include <avr/io.h>
#include <stdint.h>

#define DATA_PIN PB3  /* Data input for shift register */
#define LATCH_PIN PB2 /* Latch input for HC595 Register */
#define CLOCK_PIN PB1 /* Clock for HC595 Register */
#define RESET_PIN PB0 /* Reset HC595 Register */

#define DISPLAY_DDR DDRB   /* Display Output DDR (7 Seg) */
#define DISPLAY_PORT PORTB /* Display Port (7 Seg )*/

/* Array of digits for 7-segment */
const uint8_t digits_array[10] = {
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

void displayNumber(uint8_t digit);
