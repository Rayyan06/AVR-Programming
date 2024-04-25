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
#include <util/delay.h>

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

// Structure for encoder data
typedef struct
{
    volatile uint8_t position;
    volatile uint8_t state;
} Encoder;

// State Table for the encoder
const uint8_t state_table[8][4] = {
    {R_CW_NEXT, R_CW_BEGIN, R_CW_FINAL, R_START /* Invalid*/},           // R_CW_NEXT 00
    {R_CW_NEXT, R_CW_BEGIN, R_START /*Invalid*/, R_START},               // R_CW_BEGIN 01
    {R_CW_NEXT, R_START /* Invalid */, R_CW_FINAL, R_START | DIR_CW},    // R_CW_FINAL 02
    {R_START /* Invalid */, R_CW_BEGIN, R_CCW_BEGIN, R_START},           // R_START  03
    {R_CCW_NEXT, R_CCW_FINAL, R_CCW_BEGIN, R_START /* Invalid */},       // R_CCW_NEXT 04
    {R_CCW_NEXT, R_START /* Invalid */, R_CCW_BEGIN, R_START},           // R_CCW_BEGIN 05
    {R_CCW_NEXT, R_CCW_FINAL, R_START /* Invalid */, R_START | DIR_CCW}, // R_CCW_FINAL 06
    {R_START, R_START, R_START, R_START},                                // R_ILLEGAL 07
};

Encoder encoder = {0, 0};

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
    return (PIND & (1 << ENCODER_A_PIN) << 1) | (PIND & (1 << ENCODER_B_PIN));
}

static inline void updateEncoderState()
{
    uint8_t pinValues = readEncoderPinState();

    encoder.state = state_table[encoder.state & 0x07][pinValues];
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
}
int main(void)
{
    // ----------- Inits ----------- //

    LED_DDR = 0xFF; /* Configure LED DDR as output */
    ENCODER_PORT |=
        (1 << ENCODER_A_PIN) | (1 << ENCODER_B_PIN); /* enable pull-up resistors for encoder */
    initEncoderInterrupts();
    initUSART();

    // Initialize State
    encoder.state = 0x03;

    // ----------- Event Loop ----------- //

    while (1)
    {
        /* Do literally nothing else */
        // printByte(encoder.state);
        // printString("\r\n");
        printString("Encoder State: ");
        printByte(encoder.state);
        printString("\r\n");

        //_delay_ms(100);
    }
    return (0);
}