/*
A simple test of serial-port functionality.
Takes in a character at a time and sends it right back out,
displaying the ASCII value on the LEDs.
*/

// ------------ Preamble ------------ // 
#include <avr/io.h>
#include <util/delay.h>
#include "pinDefines.h"
#include "USART.h"

int main(void) {
    char serialCharacter;

    // ------------ Inits ----------- //
    LED_DDR = 0xFF;                 /* Set up LED's for output*/
    initUSART();
    printString("Hello World \r\n");

    // ------------ Event Loop ------------ //    
    while (1) {

        serialCharacter = receiveByte();
        transmitByte(serialCharacter);
        LED_PORT = (serialCharacter << 2);      /* display ascii/numeric value of character */
        printBinaryByte(serialCharacter);
        printString("\r\n");
    }
    return(0);
}