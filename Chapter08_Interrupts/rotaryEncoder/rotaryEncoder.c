/*

Basic Rotary Encoder Read

Read encoder data and plot on serial

*/
/*
PIN CONNECTIONS
---------------
GND -> GND
+   -> 5V
SW  -> 12
DT -> 2
CLK -> 3
*/

// ----------- Preamble ----------- //
#include "USART.h"
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/power.h>
#include <util/delay.h>

// ----------- #defines ----------- //
#define DATA_PIN PB3
#define LATCH_PIN PB2
#define CLOCK_PIN PB1
#define RESET_PIN PB0

#define DISPLAY_DDR DDRB
#define DISPLAY_PORT PORTB

// ----------- #defines ----------- //
#define ENCODER_A_PIN PD2 /* A pin */
#define ENCODER_B_PIN PD3 /* B pin */

#define ENCODER_PORT PORTD

#define LED_DDR DDRB
#define LED_PORT PORTB

#define LED0 PB0
#define LED1 PB1
#define LED2 PB2
#define LED3 PB3

#define LOG_FREQ 5 /* number of times a second to log encoder data */

#define DIR_NONE 0x00 /* No step yet */
#define DIR_CW 0x10   /* Clockwise step */
#define DIR_CCW 0x20  /* Counterclockwise step*/

#define R_START 0x3
#define R_CW_BEGIN 0x1
#define R_CW_NEXT 0x0
#define R_CW_FINAL 0x2
#define R_CCW_BEGIN 0x6
#define R_CCW_NEXT 0x4
#define R_CCW_FINAL 0x5
#define R_ILLEGAL 0x7
// ----------- Global Variables ----------- //

// ----------- Global Variables ----------- //

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

// Structure for encoder data
typedef struct
{
    volatile uint8_t position;
    volatile uint8_t lastPosition;
    volatile uint8_t state;
} Encoder;

// State Table for the encoder
const uint8_t state_table[8][4] = {
    {R_CW_NEXT, R_CW_BEGIN, R_CW_FINAL, R_START /* Invalid*/},           // R_CW_NEXT 00
    {R_CW_NEXT, R_CW_BEGIN, R_START /*Invalid*/, R_START},               // R_CW_BEGIN 01
    {R_CW_NEXT, R_START /* Invalid */, R_CW_FINAL, R_START | DIR_CW},    // R_CW_FINAL 02
    {R_START /* Invalid */, R_CW_BEGIN, R_CCW_BEGIN, R_START},           // R_START  03
    {R_CCW_NEXT, R_CCW_FINAL, R_CCW_BEGIN, R_START /* Invalid */},       // R_CCW_NEXT 04
    {R_CCW_NEXT, R_CCW_FINAL, R_START /* Invalid */, R_START | DIR_CCW}, // R_CCW_FINAL 05
    {R_CCW_NEXT, R_START /* Invalid */, R_CCW_BEGIN, R_START},           // R_CCW_BEGIN 06
    {R_START, R_START, R_START, R_START},                                // R_ILLEGAL 07
};

Encoder encoder = {0, 0, 0};

//  ----------- Functions ----------- //
static inline void initEncoderInterrupts()
{

    EIMSK |= (1 << INT0) | (1 << INT1); /* enable INT0 & INT1 */
    EICRA |= (1 << ISC00);              /* trigger INT0 when button state changes */
    EICRA |= (1 << ISC10);              /* trigger INT1 when button state changes */

    sei(); /* enable global interrupts */
}
static inline uint8_t readEncoderPinState()
{
    return (PIND & (1 << ENCODER_A_PIN) ? 0x02 : 0x00) |
           ((PIND & (1 << ENCODER_B_PIN)) ? 0x01 : 0x00);
}

static inline void updateEncoderState()
{
    uint8_t pinValues = readEncoderPinState();
    // printString("Pin Value: ");
    // printBinaryByte(pinValues);
    // printString("\r\n");
    // LED_PORT = (pinValues & 0x01) ? (LED_PORT | (1 << PB0)) : (LED_PORT & ~(1 << PB0));
    // LED_PORT = (pinValues & 0x02) ? (LED_PORT | (1 << PB1)) : (LED_PORT & ~(1 << PB1));

    encoder.state = state_table[encoder.state & 0x07][pinValues];
}

void displayNumber(uint8_t digit)
{
    DISPLAY_PORT &= ~(1 << LATCH_PIN); // Set latch off
    for (uint8_t i = 0; i < 8; i++)
    {
        DISPLAY_PORT &= ~(1 << CLOCK_PIN);               // Write the clock low
        uint8_t bit = (digits_array[digit] >> i) & 0x01; // Grab the bit at the i'th position
        DISPLAY_PORT =
            bit ? (DISPLAY_PORT | (1 << DATA_PIN))
                : (DISPLAY_PORT & ~(1 << DATA_PIN)); // Write the Data pin depending on the bit

        DISPLAY_PORT |= (1 << CLOCK_PIN); // Set the clock pin high, storing the data.
    }
    DISPLAY_PORT |= (1 << LATCH_PIN); // Set latch pin on, sending the data over
}
void printEncoderState()
{
    if ((encoder.state & 0x07) == R_CCW_BEGIN)
    {
        printString("R_CCW_BEGIN");
    }
    else if ((encoder.state & 0x07) == R_CCW_NEXT)
    {
        printString("R_CCW_NEXT");
    }
    else if ((encoder.state & 0x07) == R_CCW_FINAL)
    {
        printString("R_CCW_FINAL");
    }
    else if ((encoder.state & 0x07) == R_START)
    {
        printString("R_START");
    }
    else if ((encoder.state & 0x07) == R_CW_BEGIN)
    {
        printString("R_CW_BEGIN");
    }
    else if ((encoder.state & 0x07) == R_CW_NEXT)
    {
        printString("R_CW_NEXT");
    }
    else if ((encoder.state & 0x07) == R_CW_FINAL)
    {
        printString("R_CW_FINAL");
    }
    else
    {
        printString("ILLEGAL");
    }

    if (encoder.state & DIR_CCW)
    {
        printString(" DIR_CCW ");
    }
    else if (encoder.state & DIR_CW)
    {
        printString(" DIR_CW ");
    }
    else
    {
        printString(" DIR_NONE ");
    }

    printBinaryByte(encoder.state);
    printString("\r\n");
}
//  ----------- Interrupt Service Routines ----------- //

ISR(INT0_vect) /* on change of encoder pin A */
{
    // encoder.position = 0x1;
    updateEncoderState();
    if (encoder.state & DIR_CW)
    {
        encoder.position++;
    }
    if (encoder.state & DIR_CCW)
    {
        encoder.position--;
    }
    // printEncoderState();
}

ISR(INT1_vect) /* on change of encoder pin B*/
{
    // encoder.position = 0x2;
    updateEncoderState();
    if (encoder.state & DIR_CW)
    {
        encoder.position++;
    }
    if (encoder.state & DIR_CCW)
    {
        encoder.position--;
    }
    // printEncoderState();
}
int main(void)
{
    // ----------- Inits ----------- //

    // LED_DDR = 0xFF; /* Configure LED DDR as output */

    DISPLAY_DDR |= (1 << CLOCK_PIN) | (1 << DATA_PIN) | (1 << LATCH_PIN) |
                   (1 << RESET_PIN); // Set display DDR to output

    DISPLAY_PORT |= (1 << RESET_PIN);

    ENCODER_PORT |=
        (1 << ENCODER_A_PIN) | (1 << ENCODER_B_PIN); /* enable pull-up resistors for encoder */
    initEncoderInterrupts();
    initUSART();

    // Full Speed
    clock_prescale_set(clock_div_16);

    // Initialize State
    encoder.state = readEncoderPinState();

    // ----------- Event Loop ----------- //

    while (1)
    {
        /* Do literally nothing else */
        // printByte(encoder.state);
        // printString("\r\n");
        if (encoder.position != encoder.lastPosition)
        {
            // printByte(encoder.position);
            displayNumber(encoder.position % 10);
            // printString("\r\n");
            encoder.lastPosition = encoder.position;
        }
    }
    return (0);
}