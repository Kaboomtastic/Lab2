// TableTrafficLight.c solution to edX lab 10, EE319KLab 5
// Runs on LM4F120 or TM4C123
// Index implementation of a Moore finite state machine to operate a traffic light.  
// Daniel Valvano, Jonathan Valvano
// November 7, 2013

/* solution, do not post

 Copyright 2014 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

// east/west red light connected to PB5
// east/west yellow light connected to PB4
// east/west green light connected to PB3
// north/south facing red light connected to PB2
// north/south facing yellow light connected to PB1
// north/south facing green light connected to PB0
// pedestrian detector connected to PE2 (1=pedestrian present)
// north/south car detector connected to PE1 (1=car present)
// east/west car detector connected to PE0 (1=car present)
// "walk" light connected to PF3 (built-in green LED)
// "don't walk" light connected to PF1 (built-in red LED)
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "SysTick.h"
#include "TExaS.h"

// Declare your FSM linked structure here

#define	WaitAll	0
#define	GNor	1
#define	YNor	2
#define	YNorPe	3
#define	GEas	4
#define	YEas	5
#define	YEasPe 6
#define	GPed	7
#define	waitPed1	8
#define	waitPed2	9
#define waitPed3	10
#define	waitPed4	11
#define NtoEPause 12
#define EtoNPause 13
#define NtoPPause 14
#define EtoPPause 15
#define PtoNPause 16
#define PtoEPause 17

typedef struct States{
	uint32_t TrafficLights;
	uint32_t PedestrianLights;
	uint32_t delay;
	uint32_t NextState[8]; // 000, 001, 010, 011, 100, 101, 110, 111
}sType;

sType FSM[18]={
	
	{0x24, 0x02, 200, {WaitAll, GEas, GNor, GNor, GPed, GPed, GPed, GPed}}, 
	{0x0C, 0x02, 200, {GNor, YNor, GNor, YNor, YNorPe, YNorPe, YNorPe, YNor}},
	{0x14, 0x02, 100, {WaitAll, NtoEPause, WaitAll, NtoEPause, NtoEPause, NtoEPause, NtoEPause, NtoEPause}},
	{0x14, 0x02, 100, {NtoPPause, NtoPPause, NtoPPause, NtoPPause, NtoPPause, NtoPPause, NtoPPause, NtoPPause}},
	{0x21, 0x02, 200, {GEas, GEas, YEas, YEas, YEasPe, YEasPe, YEasPe, YEasPe}},
	{0x22, 0x02, 100, {WaitAll, WaitAll, EtoNPause, EtoNPause, EtoNPause, EtoNPause, EtoNPause, EtoNPause}},
	{0x22, 0x02, 100, {EtoPPause, EtoPPause, EtoPPause, EtoPPause, EtoPPause, EtoPPause, EtoPPause, EtoPPause}},
	{0x24, 0x08, 200, {GPed, waitPed1, waitPed1, waitPed1, GPed, waitPed1, waitPed1, waitPed1}},
	{0x24, 0x02, 50, {waitPed2, waitPed2, waitPed2, waitPed2, waitPed2, waitPed2, waitPed2, waitPed2}},
	{0x24, 0x00, 50, {waitPed3, waitPed3, waitPed3, waitPed3, waitPed3, waitPed3, waitPed3, waitPed3}},
	{0x24, 0x02, 50, {waitPed4, waitPed4, waitPed4, waitPed4, waitPed4, waitPed4, waitPed4, waitPed4}},
	{0x24, 0x00, 50, {WaitAll, PtoEPause, PtoNPause, PtoNPause, WaitAll, PtoEPause, PtoNPause, PtoNPause}},
	{0x24, 0x02, 100, {GEas, GEas, GEas, GEas, GEas, GEas, GEas, GEas}},
	{0x24, 0x02, 100, {GNor, GNor, GNor, GNor, GNor, GNor, GNor, GNor}},
	{0x24, 0x02, 100, {GPed, GPed, GPed, GPed, GPed, GPed, GPed, GPed}},
	{0x24, 0x02, 100, {GPed, GPed, GPed, GPed, GPed, GPed, GPed, GPed}},
	{0x24, 0x02, 100, {GNor, GNor, GNor, GNor, GNor, GNor, GNor, GNor}},
	{0x24, 0x02, 100, {GEas, GEas, GEas, GEas, GEas, GEas, GEas, GEas}},
};
	
void EnableInterrupts(void);
void PinsInit(void){
	volatile uint16_t delay2;
	SYSCTL_RCGC2_R |= 0x32;				// using pins B, E, and F
	delay2 = SYSCTL_RCGC2_R;
  GPIO_PORTE_DIR_R &= ~0x07;   // 5) inputs on PE2-0
  GPIO_PORTE_AFSEL_R &= ~0x07; // 6) regular function on PE1-0
  GPIO_PORTE_DEN_R |= 0x07;    // 7) enable digital on PE1-0
  GPIO_PORTB_DIR_R |= 0x3F;    // 5) outputs on PB5-0
  GPIO_PORTB_AFSEL_R &= ~0x3F; // 6) regular function on PB5-0
  GPIO_PORTB_DEN_R |= 0x3F;    // 7) enable digital on PB5-0
	GPIO_PORTF_DIR_R |= 0x0A;		 // 5) outputs on PF3 and 1
	GPIO_PORTF_AFSEL_R &= ~0x0A; // 6) regular function on PF3 and 1
	GPIO_PORTF_DEN_R |= 0x0A;		 // 7) enable digital on PF3 and 1
}
	
int main(void){ 
	volatile uint32_t delay;
	volatile uint32_t input;
	volatile uint32_t CurrentState = WaitAll;
  TExaS_Init(SW_PIN_PE210, LED_PIN_PB543210); // activate traffic simulation and set system clock to 80 MHz
  SysTick_Init();     
  EnableInterrupts();
	PinsInit();
	
	
  //FSM Engine
  while(1){
 
	GPIO_PORTB_DATA_R = FSM[CurrentState].TrafficLights;			// set lights to current state
	GPIO_PORTF_DATA_R = FSM[CurrentState].PedestrianLights;
	SysTick_Wait10ms(FSM[CurrentState].delay);								// conduct appropriate delay
	input = GPIO_PORTE_DATA_R;															// gather inputs
	CurrentState = FSM[CurrentState].NextState[input];				// set next state as current state
		
  }
}

