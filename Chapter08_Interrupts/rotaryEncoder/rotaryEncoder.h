#ifndef ROTARY_ENCODER_H
#define ROTARY_ENCODER_H

#include <stdint.h>

// ----------- #defines ----------- //
#define F_CPU 16000000UL // Assuming a 16MHz clock frequency
#define PRESCALER_VALUE 64
#define SAMPLE_RATE 50 // Interrupt every 5ms

/* Compare Value is the Timer count speed in millisecond, multiplied by the Timer Period*/
#define COMPARE_VALUE ((F_CPU / PRESCALER_VALUE) / 1000) * SAMPLE_RATE

#define NUMBER_OF_SAMPLES 2

#define ENCODER_A_PIN 2 /* PD2 pin */
#define ENCODER_B_PIN 3 /* PD3 pin */

#define ENCODER_PORT PORTD

/* Definitions for the quadrature encoder state machine */
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

// ----------- Structures ----------- //
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

// State Table for the encoder
const uint8_t state_table[8][4] = {
    {R_CW_NEXT, R_CW_BEGIN | DIR_CCW, R_CW_FINAL | DIR_CW, R_START /* Invalid*/},  // R_CW_NEXT 00
    {R_CW_NEXT | DIR_CW, R_CW_BEGIN, R_CW_FINAL /*Invalid*/, R_START | DIR_CCW},   // R_CW_BEGIN 01
    {R_CW_NEXT | DIR_CCW, R_CW_BEGIN /* Invalid */, R_CW_FINAL, R_START | DIR_CW}, // R_CW_FINAL 02
    {R_START /* Invalid */, R_CW_BEGIN | DIR_CW, R_CCW_BEGIN | DIR_CCW, R_START},  // R_START  03
    {R_CCW_NEXT, R_CCW_FINAL | DIR_CCW, R_CCW_BEGIN | DIR_CW,
     R_START /* Invalid */}, // R_CCW_NEXT 04
    {R_CCW_NEXT | DIR_CW, R_CCW_FINAL, R_CCW_BEGIN /* Invalid */,
     R_START | DIR_CCW}, // R_CCW_FINAL 05
    {R_CCW_NEXT | DIR_CCW, R_CCW_NEXT /* Invalid */, R_CCW_BEGIN,
     R_START | DIR_CW},                   // R_CCW_BEGIN 06
    {R_START, R_START, R_START, R_START}, // R_ILLEGAL 07
};

// ----------- Function Prototypes ----------- //

/* Initializes the Timer with some settings */
static inline void initTimer1(void);

/* Read the current state of the encoder pins A & B */
static inline uint8_t readEncoderPinState(void);

/* Update the state of the encoder. State is in the format of 0b000(DIRECTION_BIT)00(A_PIN)(B_PIN)*/
static inline void updateEncoderState(void);

/* Update the position of the encoder based on the state */
static inline void updateEncoderPosition(void);

static inline void printData();

static inline void loadDataIntoBuffer();

#endif // ROTARY_ENCODER_H