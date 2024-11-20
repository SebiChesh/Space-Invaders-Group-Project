/*
 * DE1SoC_SevenSeg.c
 *
 *  Created on: 12 Feb 2021
 *      Author: Harry Clegg
 *  Updated on: 24 Feb 2024
 *      Author: David Cowell
 * Modified by:	Andrea Yanez
 */

#include "DE1SoC_SevenSeg.h"

// ToDo: Add the base addresses of the seven segment display peripherals.
volatile unsigned char *sevenseg_base_lo_ptr = (volatile unsigned char *)0xFF200020;
volatile unsigned char *sevenseg_base_hi_ptr = (volatile unsigned char *)0xFF200030;

// There are four HEX displays attached to the low (first) address.
#define SEVENSEG_N_DISPLAYS_LO 4

// There are two HEX displays attached to the high (second) address.
#define SEVENSEG_N_DISPLAYS_HI 2

void DE1SoC_SevenSeg_Write(unsigned int display, unsigned char value) {
    // Select between the two addresses so that the higher level functions
    // have a seamless interface.
    if (display < SEVENSEG_N_DISPLAYS_LO) {
        // If we are targeting a low address, use byte addressing to access
        // directly.
        sevenseg_base_lo_ptr[display] = value;
    } else {
        // If we are targeting a high address, shift down so byte addressing
        // works.
        display = display - SEVENSEG_N_DISPLAYS_LO;
        sevenseg_base_hi_ptr[display] = value;
    }
}

void DE1SoC_SevenSeg_SetSingle(unsigned int display, unsigned int value) {
    // ToDo: Write the code for driving a single seven segment display here.
    // Your function should turn a real value 0-F into the correctly encoded
    // bits to enable the correct segments on the seven segment display to
    // illuminate. Use the DE1SoC_SevenSeg_Write function you created earlier
    // to set the bits of the display.

	//A switch statement is used for displaying each case (0-F) on the HEX0 display
	switch (value){
	case 0:
		DE1SoC_SevenSeg_Write(display, 0X3F); // HEX-BIN conversion: 3F is 0011 1111, illuminates segments (0-5)
	break;
	case 1:
		DE1SoC_SevenSeg_Write(display, 0X6); // HEX-BIN conversion: 6 is 0110, illuminates segments (1&2)
	break;
	case 2:
		DE1SoC_SevenSeg_Write(display, 0X5B); // HEX-BIN conversion: 5B is 0101 1011, illuminates segments (0,1,3,4,6)
	break;
	case 3:
		DE1SoC_SevenSeg_Write(display, 0X4F); // HEX-BIN conversion: 4F is 0100 1111, illuminates segments (0-3, 6)
	break;
	case 4:
		DE1SoC_SevenSeg_Write(display, 0X66); // HEX-BIN conversion: 66 is 0110 0110, illuminates segments (1,2,5,6)
	break;
	case 5:
		DE1SoC_SevenSeg_Write(display, 0X6D); // HEX-BIN conversion: 6D is 0110 1101, illuminates segments (0,2,3,5,6)
	break;
	case 6:
		DE1SoC_SevenSeg_Write(display, 0X7D); // HEX-BIN conversion: 7D is 0111 1101, illuminates segments (0, 2-6)
	break;
	case 7:
		DE1SoC_SevenSeg_Write(display, 0X7); // HEX-BIN conversion: 7 is 0111, illuminates segments (0-2)
	break;
	case 8:
		DE1SoC_SevenSeg_Write(display, 0X7F); // HEX-BIN conversion: 7F is 0111 1111, illuminates segments (0-6)
	break;
	case 9:
		DE1SoC_SevenSeg_Write(display, 0X6F); // HEX-BIN conversion: 6F is 0110 1111, illuminates segments (0-3, 5,6)
	break;
	case 0XA:
		DE1SoC_SevenSeg_Write(display, 0X77); // HEX-BIN conversion: 77 is 0111 0111, illuminates segments (0-2, 4-6)
	break;
	case 0XB:
		DE1SoC_SevenSeg_Write(display, 0X7C); // HEX-BIN conversion: 6F is 0111 1100, illuminates segments (2-6)
	break;
	case 0XC:
		DE1SoC_SevenSeg_Write(display, 0X39); // HEX-BIN conversion: 39 is 0011 1001, illuminates segments (0, 3-5)
	break;
	case 0XD:
		DE1SoC_SevenSeg_Write(display, 0X5E); // HEX-BIN conversion: 5E is 0101 1110, illuminates segments (1-4, 6)
	break;
	case 0XE:
		DE1SoC_SevenSeg_Write(display, 0X79); // HEX-BIN conversion: 79 is 0111 1001, illuminates segments (0, 3-6)
	break;
	case 0XF:
		DE1SoC_SevenSeg_Write(display, 0X71); // HEX-BIN conversion: 71 is 0111 0001, illuminates segments (0, 4-6)
	break;
	default:
		DE1SoC_SevenSeg_Write(display, 0x40); // HEX-BIN conversion: 40 is 0100 0000, illuminates segment 6
	}
}

void DE1SoC_SevenSeg_SetDoubleHex(unsigned int display, unsigned int value) {
    // ToDo: Write the code for setting a pair of seven segment displays
    // here. Good coding practice suggests your solution should call
    // DE1SoC_SevenSeg_SetSingle() twice.
    // This function should show the first digit of a HEXADECIMAL number on
    // the specified 'display', and the second digit on the display to
    // the left of the specified display.

    /** Some examples:
     *
     *    input | output | HEX(N+1) | HEX(N)
     *    ----- | ------ | -------- | ------
     *        5 |     05 |        0 |      5
     *       30 |     1E |        1 |      E
     *     0x60 |     60 |        6 |      0
     */

	// Variables are created for each digit of the display
	int hexn; // variable for right-most digit
	int hexn1; // variable for left-most digit

	// An if-else statement will print the symbols 00-FF and a dash for out of range values
	if (value < 256) {
		hexn = value % 16; // Modulus gives the residue of division by 16
		hexn1 = value >> 4; // Operator >> right-shifts 4 bits
	}
	else {
		hexn = 0X2F; // Value greater than 0XF will return a dash for right digit
		hexn1 = 0X2F; // Value greater than 0XF will return a dash for left digit
	}

	// Calling the function twice DE1SoC_SevenSeg_SetSingle to print the two digits
	DE1SoC_SevenSeg_SetSingle(display, hexn); // Calling the function to print right digit
	DE1SoC_SevenSeg_SetSingle(display+1, hexn1); // Calling the function to print left digit
}

void DE1SoC_SevenSeg_SetDoubleDec(unsigned int display, unsigned int value) {
    // ToDo: Write the code for setting a pair of seven segment displays
    // here. Good coding practice suggests your solution should call
    // DE1SoC_SevenSeg_SetSingle() twice.
    // This function should show the first digit of a DECIMAL number on
    // the specified 'display', and the second digit on the display to
    // the left of the specified display.

    /** Some examples:
     *
     *	  input | output | HEX(N+1) | HEX(N)
     *    ----- | ------ | -------- | ------
     *        5 |     05 |        0 |      5
     *       30 |     30 |        3 |      0
     *     0x60 |     96 |        9 |      6
     */

	// Variables are created for each digit of the display
	int digit1;  // variable for right-most digit
	int digit2;  // variable for left-most digit

	// An if-else statement will print the symbols 00-99 and a dash for out of range values
	if (value < 100) {
		digit1 = value % 10; // Modulus gives the residue of division by 10
		digit2 = value / 10; // Division counts 1 every 10 iterations of the button
	}
	else {
		digit1 = 0X2F; // Value greater than 0XF will return a dash for right digit
		digit2 = 0X2F; // Value greater than 0XF will return a dash for left digit
	}

	// Calling the function twice DE1SoC_SevenSeg_SetSingle to print the two digits
	DE1SoC_SevenSeg_SetSingle(display, digit1); // Calling the function to print right digit
	DE1SoC_SevenSeg_SetSingle(display+1, digit2); // Calling the function to print left digit
}
