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
#include <util/setbaud.h>

// ----------- Global Variables ----------- //
volatile uint16_t time;

// Initialize our encoder with 0's
volatile Encoder encoder = {0, 0, 0};

// Initialize our buffer
volatile circular_buffer_t tx_buffer = {
    .head = 0, .tail = 0, .data = {{0, 0}, {0, 0}, {0, 0}, {0, 0}}};
volatile uint8_t loggingComplete = 0;

void initBuffer(volatile circular_buffer_t *buffer)
{
    buffer->head = 0;
    buffer->tail = 0;
}
// Check if the buffer is empty
uint8_t buffer_is_empty(volatile circular_buffer_t *buffer)
{
    return buffer->head == buffer->tail;
}
//   ----------- Functions ----------- //
void initBasicUSART(void)
{                         /* requires BAUD */
    UBRR0H = UBRRH_VALUE; /* defined in setbaud.h */
    UBRR0L = UBRRL_VALUE;
#if USE_2X
    UCSR0A |= (1 << U2X0);
#else
    UCSR0A &= ~(1 << U2X0);
#endif
    /* Enable USART transmitter/receiver */
    UCSR0B = (1 << TXEN0) | (1 << RXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); /* 8 data bits, 1 stop bit */

    // Initialize USART and enable transmit complete interrupt
    UCSR0B |= (1 << TXCIE0); // Enable USART transmit complete interrupt
}

/* Timer 1 is for timing the difference between interrupt triggers, and the real time with CTC */
static inline void initTimer1(void)
{
    /* Timer 1 is the 16 Bit timer */
    TCCR1B |= (1 << WGM12); /* CTC Mode */
    /* Normal mode (default), just counting */
    TCCR1B |= (1 << CS11) | (1 << CS10); /* Set the CPU Prescaler at /64 prescale, 16 Mhz / 64 =
                                            250000 ticks per second or 250 ticks per millisecond */
    // Set the compare value
    OCR1A = COMPARE_VALUE;
    // // Enable Compare Match A interrupt
    TIMSK1 |= (1 << OCIE1A);
}

static inline void initButton(void)
{

    BUTTON_DDR &= ~(1 << BUTTON);
    BUTTON_PORT |= (1 << BUTTON); /* Enable pull up resistor */

    PCICR |= (1 << PCIE0);   // Enable pin change interrupt for PCINT0 to PCINT7
    PCMSK0 |= (1 << PCINT0); // Enable Mask for PCINT0
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
    encoder.state = state_table[encoder.state & 0x07][readEncoderPinState()];
}

//  ----------- Interrupt Service Routines ----------- //
static inline void updateEncoderPosition()
{
    if (encoder.state & DIR_CW)
    {
        printString("++");
        position = position + 2;
    }
    else if (encoder.state & DIR_CCW)
    {
        printString("--");
        position = position - 2;
    }

    printWord(position); // Print low byte
    printString(" ");
    printBinaryByte(encoder.state);
    printString("\r\n");
}

/*
static inline void endLogging()
{
    // Close Interrupts
    // cli();
    transmitByte(0x03); // ETX character
    transmitByte('\n');
}
*/

void onEncoderInterrupt()
{
    updateEncoderState();
    updateEncoderPosition();

    // sDataPoint datapoint = {TCNT1, encoder.position & 0xFF};
    // USART_Transmit(datapoint);
}
ISR(INT0_vect) /* on change of encoder pin A */
{

    onEncoderInterrupt();
}

ISR(INT1_vect) /* on change of encoder pin B*/
{
    onEncoderInterrupt();
}

// ISR(PCINT0_vect)
// {
//     // Debounce first
//     if ((BUTTON_PIN & (1 << BUTTON))) /* If the button has just been pressed */
// {
//     _delay_ms(DEBOUNCE_TIME);
//     if ((BUTTON_PIN & (1 << BUTTON))) /* Now if it genuinely got pressed */
//     {
//         /* Start the counter again */
//         sei();
//         loggingComplete = 0;
//         TCNT1 = 0;
//     }
// }
// }

/* Timer1 Compare A match Interrupt Service Routine */
// ISR(TIMER1_COMPA_vect)
//{

// DataPoint datapoint = {COMPARE_VALUE, encoder.position & 0xFF};
//  USART_Transmit(datapoint);

// Enable the USART Data Transmit Ready Interrupt, to trigger ISR
// UCSR0B |= (1 << UDRIE0);

//   PORTB ^= (1 << PB1);
//}

/* USART Data Register Empty Interrupt Service Routine
ISR(USART_UDRE_vect)
{
    static uint8_t byte_index = 0;
    static DataPoint current_data;

    // Check if the buffer is not empty
    if (byte_index == 0)
    {
        if (buffer_is_empty(&tx_buffer))
        {
            UCSR0B &= ~(1 << UDRIE0);
            return;
        }
        // Read the data from the buffer, only do this once initially
        current_data = buffer_get(&tx_buffer);
    }


        switch (byte_index)
        {
        case 0:
            printByte(current_data.pos_H);
            break;
        case 1:
            printByte(current_data.pos_L);
            break;
        case 2:
            printByte(' ');
            break;
        case 3:
            printByte(current_data.time_H);
            break;
        case 4:
            printByte(current_data.time_L);
            break;
        case 5:
            printByte('\r');
            break;
        case 6:
            printByte('\n');
            break;
        }


byte_index = (byte_index + 1) % 7;
if (byte_index == 0 && buffer_is_empty(&tx_buffer))
{
    UCSR0B &= ~(1 << UDRIE0);
}
}
*/
ISR(BADISR_vect)
{

    for (;;)
        UDR0 = '!';
}
int main(void)
{
    // ----------- Inits ----------- //

    DDRB |= (1 << PB1); /* Configure LED DDR as output */

    ENCODER_PORT |=
        (1 << ENCODER_A_PIN) | (1 << ENCODER_B_PIN); /* enable pull-up resistors for encoder */

    // initButton();
    initBasicUSART();
    // initTimer1();
    initEncoderInterrupts();
    sei(); /* enable global interrupts */

    //  Full Speed
    // clock_prescale_set(clock_div_1);

    //  Initialize State with a read
    encoder.state = readEncoderPinState();

    // Initialize the buffer head and tail
    initBuffer(&tx_buffer);

    // ----------- Event Loop ----------- //

    while (1)
    {

        //     /*
        //     if ((TCNT1 > DATA_END_THRESHOLD))
        //     {
        //         TCNT1 = 0;
        //         endLogging();
        //         loggingComplete = 1;
        //     }
        //     */
        // }
        //}
        // printString("]\r\n");
    }
    return 0;
}