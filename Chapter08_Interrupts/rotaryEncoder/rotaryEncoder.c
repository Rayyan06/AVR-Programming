/*

Basic Rotary Encoder Read and display

Read encoder data and display on 7 Segment through shift register

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
#include "LiquidCrystal.h"
#include "USART.h"
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/power.h>
#include <util/delay.h>

// ----------- #defines ----------- //
#define F_CPU 16000000UL // Assuming a 16MHz clock frequency
#define PRESCALER_VALUE 64
#define SAMPLE_RATE 5 // Interrupt every 5ms

#define NUMBER_OF_SAMPLES 100

/* Compare Value is the Timer count speed in millisecond, multiplied by the Timer Period*/
#define COMPARE_VALUE ((F_CPU / PRESCALER_VALUE) / 1000) * SAMPLE_RATE
// ----------- #defines ----------- //
#define ENCODER_A_PIN PD2 /* A pin */
#define ENCODER_B_PIN PD3 /* B pin */

#define LCD_RS_PIN PB4
#define LCD_ENABLE_PIN PB3

#define LCD_DATA_PIN_0 PD4
#define LCD_DATA_PIN_1 PD5
#define LCD_DATA_PIN_2 PD6
#define LCD_DATA_PIN_3 PD7

#define THETA 0xF2
#define ENCODER_PORT PORTD

/* Definitions for the quadrature encoder state machine */
// Explanation: buxtronic

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
    volatile uint16_t position;
    volatile uint16_t lastPosition;
    volatile uint8_t state;
    volatile uint8_t lastState;
    volatile float angle;
    volatile float lastAngle;
    volatile float velocity;
} Encoder;

volatile uint16_t time;

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

// Initialize our encoder with 0's
Encoder encoder = {0, 0, 0, 0};

// Initialize our LCD
LCD lcd = {};

uint8_t position_data[NUMBER_OF_SAMPLES];
uint8_t *dataIndex = position_data;

const uint8_t *lastIndex = &position_data[NUMBER_OF_SAMPLES - 1];

uint8_t loggingComplete = 0;
//  ----------- Functions ----------- //

static inline void initTimer1(void)
{
    /* Timer 1 is the 16 Bit timer */
    TCCR1B |= (1 << WGM12);              /* CTC Mode */
    TCCR1B |= (1 << CS11) | (1 << CS10); /* Set the CPU Prescaler at /64 prescale, 16 Mhz / 64 =
                                            250000 ticks per second or 4 ticks per millisecond */
    // Set the compare value
    OCR1A = COMPARE_VALUE;
    // Enable Compare Match A interrupt
    TIMSK1 |= (1 << OCIE1A);
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
/*
char *printEncoderState()
{
    switch (encoder.state & 0x07)
    {
    case R_CCW_BEGIN:
        return "CCW_BEGIN";
        break;
    case R_CCW_NEXT:
        return "CCW_NEXT ";
        break;
    case R_CCW_FINAL:
        return "CCW_FINAL";
        break;
    case R_START:
        return "START    ";
        break;
    case R_CW_BEGIN:
        return "CW_BEGIN ";
        break;
    case R_CW_NEXT:
        return "CW_NEXT  ";
        break;
    case R_CW_FINAL:
        return "CW_FINAL ";
        break;
    default:
        return "ILLEGAL  ";
    }



    if (encoder.state & DIR_CCW)
    {
        return "DIR_CCW ";
    }
    else if (encoder.state & DIR_CW)
    {
        return "DIR_CW  ";
    }
    else
    {
        return "DIR_NONE";
    }

}
*/
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
    }

    // encoder.angle = encoder.position * 0.0125664;
}

void sendData()
{
    for (uint8_t i = 0; i < NUMBER_OF_SAMPLES; i++)
    {
        transmitByte(position_data[i]);
        transmitByte('\r');
        transmitByte('\n');
        _delay_ms(100);
    }
}
/*
static inline void printTime(uint16_t time)
{
    // setCursor(&lcd, 1, 3);
    // printInt16(&lcd, time >> 4);
}
*/
ISR(INT0_vect) /* on change of encoder pin A */
{
    // encoder.position = 0x1;
    updateEncoderState();
    updateEncoderPosition();
    // printEncoderState();
}

ISR(INT1_vect) /* on change of encoder pin B*/
{
    // encoder.position = 0x2;
    updateEncoderState();
    updateEncoderPosition();
    // printEncoderState()
}
ISR(TIMER1_COMPA_vect)
{
    if (!(dataIndex == lastIndex)) /* If we haven't reached the last element */
    {
        *dataIndex = encoder.position;
        dataIndex++;
        PORTB ^= (1 << PB1);
    }
    else
    {
        loggingComplete = 1;
    }
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

    // initLCD(&lcd, 1, 2, LCD_RS_PIN, LCD_ENABLE_PIN, LCD_DATA_PIN_0, LCD_DATA_PIN_1,
    // LCD_DATA_PIN_2,
    //        LCD_DATA_PIN_3);

    DDRB |= (1 << PB1);
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
        /*
        printString("Current Position: ");
        printByte(*dataIndex);
        printString("\r\n");
        _delay_ms(50);
*/
        if (loggingComplete)
        {
            cli();
            sendData();

            loggingComplete = 0;
            TCNT1 = 0;
            dataIndex = position_data;
            sei();
        }
        // printByte(encoder.state);
        // printString("\r\n");
        // if (encoder.position != encoder.lastPosition)
        // {
        //     // printByte(encoder.position);
        //     displayNumber(encoder.position % 10);
        //     // printString("\r\n");

        //     encoder.lastPosition = encoder.position;
        // }
        // transmitByte(encoder.position);
        // printByte(encoder.position);
        // printString("\r\n");
        /*
        _delay_ms(50);
        if (encoder.position != encoder.lastPosition)
        {
            setCursor(&lcd, 0, 4);
            printUint16(&lcd, encoder.position);
            print(&lcd, "  ");
            setCursor(&lcd, 1, 3);
            printTime(time);
            print(&lcd, "  ");
            encoder.lastPosition = encoder.position;
            // encoder.lastAngle = encoder.angle;
        }
        */
        /*
        if (encoder.state != encoder.lastState)
        {
            setCursor(&lcd, 1, 14);
            send(&lcd, (encoder.state & 0x01) ? 0xFF : ' ', 1);
            send(&lcd, (encoder.state & 0x02) ? 0xFF : ' ', 1);
            setCursor(&lcd, 1, 4);
            print(&lcd, printEncoderState());
            encoder.lastState = encoder.state;
        }*/

        // clear(&lcd);
    }
    return (0);
}