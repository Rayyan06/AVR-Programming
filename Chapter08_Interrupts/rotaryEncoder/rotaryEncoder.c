/*

Basic Rotary Encoder Log and Send

Read encoder data

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
#include "rotaryEncoder.h"
#include "USART.h"
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/power.h>
#include <util/delay.h>

// ----------- Global Variables ----------- //
volatile uint16_t time;

// Initialize our encoder with 0's
volatile Encoder encoder = {0, 0, 0, 0};

typedef struct
{
    uint16_t time;  /* In milliseconds */
    uint16_t angle; /* In Degrees */
} DataPoint;

// TODO: Set up circular buffer
// Buffer for encoder data
DataPoint encoder_buffer[NUMBER_OF_SAMPLES];
volatile DataPoint *buffer_index = encoder_buffer;
volatile uint8_t buffer_full = 0; // Flag to indicate if the buffer is full
const DataPoint *last_index = &encoder_buffer[NUMBER_OF_SAMPLES - 1];

// DataPoint position_data[NUMBER_OF_SAMPLES];
// volatile DataPoint *positionDataIndex = position_data;

// const DataPoint *lastIndex = &position_data[NUMBER_OF_SAMPLES - 1];

// volatile uint8_t loggingComplete = 0;
//   ----------- Functions ----------- //

static inline void initTimer1(void)
{
    /* Timer 1 is the 16 Bit timer */
    // TCCR1B |= (1 << WGM12);              /* CTC Mode */
    /* Normal mode (default), just counting */
    TCCR1B |= (1 << CS11) | (1 << CS10); /* Set the CPU Prescaler at /64 prescale, 16 Mhz / 64 =
                                            250000 ticks per second or 250 ticks per millisecond */
    // Set the compare value
    // OCR1A = COMPARE_VALUE;
    // // Enable Compare Match A interrupt
    // TIMSK1 |= (1 << OCIE1A);
}
static inline void initEncoderInterrupts()
{

    EIMSK |= (1 << INT0) | (1 << INT1); /* enable INT0 & INT1 */
    EICRA |= (1 << ISC00);              /* trigger INT0 when button state changes */
    EICRA |= (1 << ISC10);              /* trigger INT1 when button state changes */
}
static inline uint8_t readEncoderPinState()
{
    uint8_t pin_a = (PIND & (1 << ENCODER_A_PIN)) >> (ENCODER_A_PIN - 1);
    uint8_t pin_b = (PIND & (1 << ENCODER_B_PIN)) >> ENCODER_B_PIN;

    return pin_a | pin_b;

    /*
    return (PIND & (1 << ENCODER_A_PIN) ? 0x02  : 0x00) |
           ((PIND & (1 << ENCODER_B_PIN)) ? 0x01 : 0x00);
    */
}

static inline void updateEncoderState()
{
    // printString("Pin Value: ");
    // printBinaryByte(pinValues);
    // printString("\r\n");
    // LED_PORT = (pinValues & 0x01) ? (LED_PORT | (1 << PB0)) : (LED_PORT & ~(1 << PB0));
    // LED_PORT = (pinValues & 0x02) ? (LED_PORT | (1 << PB1)) : (LED_PORT & ~(1 << PB1));

    encoder.state = state_table[encoder.state & 0x07][readEncoderPinState()];
}

//  ----------- Interrupt Service Routines ----------- //

static inline void updateEncoderPosition()
{
    if (encoder.state & DIR_CW)
    {
        encoder.position++;
    }
    else if (encoder.state & DIR_CCW)
    {
        encoder.position--;
    }
    else
    {
        // DIR_NONE, don't change position
    }
}

static inline void printData()
{
    for (uint8_t i = 0; i < NUMBER_OF_SAMPLES - 1; i++)
    {
        printWord(encoder_buffer[i].angle);
        transmitByte('\r');
        printWord(encoder_buffer[i].time);
        transmitByte('\r');
        transmitByte('\n');
    }
    buffer_index = encoder_buffer;
    buffer_full = 0;
}

static inline void loadDataIntoBuffer()
{
    // As long as we havent't reached the last index

    if (!(buffer_index == last_index))
    {
        buffer_index->angle = encoder.position;
        buffer_index->time = TCNT1;
        buffer_index++;
    }
    else
    {
        buffer_full = 1; // Buffer is full
    }
    TCNT1 = 0;
}
ISR(INT0_vect) /* on change of encoder pin A */
{
    // encoder.position = 0x1;
    updateEncoderState();
    updateEncoderPosition();
    // logTime();
    //  printEncoderState();
    // loadDataIntoBuffer();
    // printEncoderState();
}

ISR(INT1_vect) /* on change of encoder pin B*/
{
    updateEncoderState();
    updateEncoderPosition();
    // logTime();

    // printEncoderState()
    loadDataIntoBuffer();
    // printEncoderState();
}

int main(void)
{
    // ----------- Inits ----------- //

    // LED_DDR = 0xFF; /* Configure LED DDR as output */

    ENCODER_PORT |=
        (1 << ENCODER_A_PIN) | (1 << ENCODER_B_PIN); /* enable pull-up resistors for encoder */

    initTimer1();
    initEncoderInterrupts();
    initUSART();

    sei(); /* enable global interrupts */

    //  Full Speed
    clock_prescale_set(clock_div_1);

    //  Initialize State
    encoder.state = readEncoderPinState();

    /*print(&lcd, "Pos: ");

    setCursor(&lcd, 1, 0);
    send(&lcd, THETA, 1); // Write 'theta' symbol
    print(&lcd, ": ");
*/
    // ----------- Event Loop ----------- //

    while (1)
    {
        if (buffer_full)
            printData();
    }
    return (0);
}