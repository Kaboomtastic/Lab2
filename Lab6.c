// Lab6.c
// Runs on LM4F120 or TM4C123
// Use SysTick interrupts to implement a 4-key digital piano
// MOOC lab 13 or EE319K lab6 starter
// Program written by: put your names here
// Date Created: 3/6/17 
// Last Modified: 3/6/17 
// Lab number: 6
// Hardware connections
// TO STUDENTS "REMOVE THIS LINE AND SPECIFY YOUR HARDWARE********


#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "Sound.h"
#include "Piano.h"
#include "TExaS.h"

// basic functions defined at end of startup.s
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts

uint8_t power = 0;
uint32_t powerDuration = 0;
uint8_t modifier = 1;
uint8_t count = 0;
uint8_t count2 = 0;


typedef struct note{
  uint16_t frequency;
  uint16_t delay; //clock speed/ frequency
  uint16_t power [40];
}note;

typedef struct song{
  note Notes [40];
  uint32_t noteDuration[40];
  uint16_t numberOfNotes;
} song;

song songOfStorms;

uint8_t CurrentNote;
note Notes []  ={ {440,4545,{32,37,42,47,51,55,58,61,62,63,63,63,62,61,58,55,51,47,42,37,32,27,22,17,13,9,6,3,2,0,0,0,2,3,6,9,13,17,22,27}},
{494,4048,{32,37,42,47,51,55,58,61,62,63,63,63,62,61,58,55,51,47,42,37,32,27,22,17,13,9,6,3,2,0,0,0,2,3,6,9,13,17,22,27}},
 {523,3824,{32,37,42,47,51,55,58,61,62,63,63,63,62,61,58,55,51,47,42,37,32,27,22,17,13,9,6,3,2,0,0,0,2,3,6,9,13,17,22,27}},
	{0,10000,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}}};
void SysTick_Handler(){    //this is the interrupt routine
  GPIO_PORTB_DATA_R = Notes[CurrentNote].power[count];
  //NVIC_ST_CTRL_R = NVIC_ST_CTRL_R & 0xFFFFFFD;
  NVIC_ST_CURRENT_R = Notes[CurrentNote].delay;
  if(count < 40){
    count++;
  }else{
    count = 0;
  }
}

void PlaySound(uint8_t a){
  CurrentNote = a;
  count = 0;
	SysTick_Handler();
}

/*void Timer0A_Handler(){
  PlaySound( songOfStorms.Notes[count2]);
  count2++;
  S = songOfStorms.noteDuration[count2];
  if(count2 == songOfStorms.numberOfNotes){
    count2 = 0;
  }
} */


int main(void){      
  TExaS_Init(SW_PIN_PE3210,DAC_PIN_PB3210,ScopeOn);    // bus clock at 80 MHz
  Piano_Init();
  Sound_Init(0);
  // other initialization
  EnableInterrupts();
  while(1){ 
		uint32_t input;
		input = Piano_In();
		switch(input){
			case 0x00: PlaySound(3); break;
			case 0x01: PlaySound(0); break;
			case 0x02: PlaySound(1); break;
			case 0x04: PlaySound(2); break;
		}while(input != 0){
			input = Piano_In();
		}
  }    
}
