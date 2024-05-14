#include <avr/io.h>
#include <stdint.h>

#define LCD_CLEAR_DISPLAY 0x01
#define LCD_RETURN_HOME 0x02

#define LCD_DISPLAY_CTRL 0x08

#define LCD_DISPLAY_ON 0x04
#define LCD_DISPLAY_OFF 0x00

#define LCD_CURSOR_ON 0x02
#define LCD_CURSOR_OFF 0x00

#define LCD_BLINK_ON 0x01
#define LCD_BLINK_OFF 0x00

#define LCD_FUNCTION_SET 0x20
#define LCD_8BIT_MODE 0x10
#define LCD_4BIT_MODE 0x00

#define LCD_2LINE 0x08
#define LCD_1LINE 0x00

#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

#define LCD_SETDDRAM_ADDRESS 0x80

#define LCD_POWER_ON_DELAY_TIME 50         /* milliseconds(ms) */
#define LCD_FUNCTION_SET_ATTEMPT_TIME 4500 /* microseconds(us) */
#define LCD_DEBUG_DELAY _delay_ms(100)     /* milliseconds (ms) */
#define LCD_DEBUG_CLOCK_DELAY _delay_ms(300)

#define LCD_DATA_DDR DDRD
#define LCD_DATA_PORT PORTD

#define LCD_OTHER_DDR DDRB
#define LCD_OTHER_PORT PORTB

/* LCD data structure */
typedef struct
{
    uint8_t _rs_pin;
    uint8_t _enable_pin;
    uint8_t _data_pins[4];

    uint8_t _displayfunction;
    uint8_t _displaycontrol;
    uint8_t _displaymode;
} LCD;

/* Initializes the LCD with the pins */
void initLCD(LCD *lcd, uint8_t fourbitmode, uint8_t lines, uint8_t rs, uint8_t enable, uint8_t d5,
             uint8_t d6, uint8_t d7, uint8_t d8);
// hi
/*
Sends a data or instruction to the LCD
    value: what to send
    mode: 0 for Command, 1 for Data
*/
void send(LCD *lcd, uint8_t value, uint8_t mode);

void setCursor(LCD *lcd, uint8_t row, uint8_t col);

void command(LCD *lcd, uint8_t value);

/* Pulse the Enable PIN on the LCD to post the command/data */
void pulseEnable(LCD *lcd);

/* Send and write are similar, write actually does the "writing", while send sets the mode and calls
 * write() */
void write(LCD *lcd, uint8_t value);

/* Prints a string to the LCD screen */
void print(LCD *lcd, const char string[]);

/* Prints a number to the LCD screen */
void printNumber(LCD *lcd, uint8_t n);

/* prints a int16_t to the lcd screen */
void printInt16(LCD *lcd, int16_t n);

void printFloat(LCD *lcd, float n, uint8_t digits);

void printUint16(LCD *lcd, uint16_t n);

/* Clears the display */
void clear(LCD *lcd);

void home(LCD *lcd);
