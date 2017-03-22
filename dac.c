// dac.c
// This software configures DAC output
// Runs on LM4F120 or TM4C123
// Program written by: put your names here
// Date Created: 3/6/17 
// Last Modified: 3/6/17 
// Lab number: 6
// Hardware connections
// TO STUDENTS "REMOVE THIS LINE AND SPECIFY YOUR HARDWARE********

#include <stdint.h>
#include "tm4c123gh6pm.h"
// Code files contain the actual implemenation for public functions
// this file also contains an private functions and private data

// **************DAC_Init*********************
// Initialize 6-bit DAC, called once 
// Input: none
// Output: none
void DAC_Init(void){
	volatile uint32_t delay;
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOB;	// activate port B
	delay = SYSCTL_RCGC2_R;								// allow time to finish
	GPIO_PORTB_AMSEL_R &= ~0x07;					// no analog
	GPIO_PORTB_PCTL_R &= ~0x00000FFF;			// regular GPIO function
	GPIO_PORTB_DIR_R |= 0x3F;							// make PB5-0 out
	GPIO_PORTB_AFSEL_R &= ~0x3F;					// disable alternate function on PB5-0
	GPIO_PORTB_DEN_R |= 0x3F;							// enable digital I/O on PB5-0
}

// **************DAC_Out*********************
// output to DAC
// Input: 4-bit data, 0 to 15 
// Output: none
void DAC_Out(uint32_t data){
	GPIO_PORTB_DATA_R = data;
}
