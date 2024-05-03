/*
1. Display clear
2. Function set:
   DL = 0; 4-bit interface data
   N = 0; 1-line display
   F = 0; 5x8 dot character font
3. Display on/off control:
   D = 0; Display off
   C = 0; Cursor off
   B = 0; Blinking off
4. Entry mode set:
   I/D = 1; Increment by 1
   S = 0; No shift
   */

#include "LiquidCrystal.h"
#include <util/delay.h>

void initLCD(LCD *lcd, uint8_t fourbitmode, uint8_t lines, uint8_t rs, uint8_t enable, uint8_t d5,
             uint8_t d6, uint8_t d7, uint8_t d8)
{

    lcd->_data_pins[0] = d5;
    lcd->_data_pins[1] = d6;
    lcd->_data_pins[2] = d7;
    lcd->_data_pins[3] = d8;

    lcd->_enable_pin = enable;
    lcd->_rs_pin = rs;

    lcd->_displaycontrol = LCD_DISPLAY_CTRL | LCD_DISPLAY_ON | LCD_CURSOR_OFF | LCD_BLINK_OFF;

    lcd->_displayfunction = LCD_1LINE | LCD_5x8DOTS;
    lcd->_displayfunction |= fourbitmode ? LCD_4BIT_MODE : LCD_8BIT_MODE;

    if (lines > 1)
    {
        lcd->_displayfunction |= LCD_2LINE;
    }
    // Set everything to output
    LCD_OTHER_DDR |= (1 << lcd->_enable_pin) | (1 << lcd->_rs_pin);

    for (int i = 0; i < 4; i++)
    {
        LCD_DATA_DDR |= (1 << lcd->_data_pins[i]);
    }
    // Wait
    _delay_ms(LCD_POWER_ON_DELAY_TIME);

    // Set RS & enable low to begin commands
    LCD_OTHER_PORT &= ~(1 << lcd->_rs_pin);
    LCD_OTHER_PORT &= ~(1 << lcd->_enable_pin);

    // we start in 8bit mode, try to set 4 bit mode
    write(lcd, 0x03);
    _delay_us(LCD_FUNCTION_SET_ATTEMPT_TIME); // wait min 4.1ms

    // second try
    write(lcd, 0x03);
    _delay_us(LCD_FUNCTION_SET_ATTEMPT_TIME); // wait min 4.1ms

    // third go!
    write(lcd, 0x03);
    _delay_us(150); // wait min 100ms

    // 4 Bit mode
    write(lcd, 0x02);

    // Set functions
    command(lcd, LCD_FUNCTION_SET | lcd->_displayfunction);

    // Turn Display On
    command(lcd, lcd->_displaycontrol);

    // Clear Display
    clear(lcd);

    // Return Home
    home(lcd);
}

/* Send either data to write or a command */
void send(LCD *lcd, uint8_t value, uint8_t mode)
{
    /* Set the RS Pin depending on the mode */
    LCD_OTHER_PORT =
        mode ? LCD_OTHER_PORT | (1 << lcd->_rs_pin) : LCD_OTHER_PORT & ~(1 << lcd->_rs_pin);

    /* Write the bits to the data */
    write(lcd, (value >> 4));
    write(lcd, value);
}

void setCursor(LCD *lcd, uint8_t row, uint8_t col)
{
    const uint8_t rowOffsets[] = {0x00, 0x40};

    command(lcd, LCD_SETDDRAM_ADDRESS | (col + rowOffsets[row]));
    _delay_ms(20);
}
void pulseEnable(LCD *lcd)
{
    LCD_OTHER_PORT &= ~(1 << lcd->_enable_pin); // Turn off enable pin
    _delay_us(1);                               // wait

    LCD_OTHER_PORT |= (1 << lcd->_enable_pin);  // Turn on enable pin
    _delay_us(1);                               // enable pulse must be >450 ns
    LCD_OTHER_PORT &= ~(1 << lcd->_enable_pin); // Turn off enable pin
    _delay_us(100);                             // commands need >37 us to settle
}

void write(LCD *lcd, uint8_t value)
{

    for (int i = 0; i < 4; i++)
    {
        /* Write the value to the 4 data pins */
        LCD_DATA_PORT = ((value >> i) & 0x01) ? LCD_DATA_PORT | (1 << lcd->_data_pins[i])
                                              : LCD_DATA_PORT & ~(1 << lcd->_data_pins[i]);
    }
    pulseEnable(lcd);
}

void command(LCD *lcd, uint8_t value)
{
    send(lcd, value, 0); // 0 for command
}

void clear(LCD *lcd)
{
    command(lcd, LCD_CLEAR_DISPLAY);
    _delay_ms(20); // Wait
}

void home(LCD *lcd)
{
    command(lcd, LCD_RETURN_HOME);
    _delay_ms(20); // Wait
}
void print(LCD *lcd, const char string[])
{

    uint8_t i = 0;

    while (string[i])
    {
        send(lcd, string[i], 1);
        i++;
    }
}

void printNumber(LCD *lcd, uint8_t n)
{
    char buffer[8 * sizeof(uint8_t) + 1];    // Allow 1 more for null terminator
    char *str = &buffer[sizeof(buffer) - 1]; // Pointer to last in buffer

    *str = '\0';

    do
    {

        *--str = '0' + n % 10;
        n /= 10;
    } while (n);

    print(lcd, str);
}
