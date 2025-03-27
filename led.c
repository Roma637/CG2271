#include "MKL25Z4.h"                    // Device header
#include "cmsis_os2.h"                  // ::CMSIS:RTOS2
#include "led.h"

uint8_t greenLeds[8] = {LED_G3, LED_G4, LED_G5, LED_G6, LED_G7, LED_G8, LED_G9, LED_G10};
//uint8_t redLeds[8] = {LED_R3, LED_R4, LED_R5, LED_R12, LED_R13, LED_R14, LED_R15, LED_R16};

static void delay(volatile uint32_t nof) {
	while(nof!=0) {
		__asm("NOP");
		nof--;
	}
}

void initLED(void) {
  // Enable Clock to PORTC
  SIM->SCGC5 |= (SIM_SCGC5_PORTC_MASK) | (SIM_SCGC5_PORTA_MASK);
   
  // Configure MUX settings for rear led
//  PORTA->PCR[LED_R3] &= ~PORT_PCR_MUX_MASK;
//  PORTA->PCR[LED_R3] |= PORT_PCR_MUX(1);
//  PORTA->PCR[LED_R4] &= ~PORT_PCR_MUX_MASK;
//  PORTA->PCR[LED_R4] |= PORT_PCR_MUX(1);
//  PORTA->PCR[LED_R5] &= ~PORT_PCR_MUX_MASK;
//  PORTA->PCR[LED_R5] |= PORT_PCR_MUX(1);
  PORTA->PCR[LED_R12] &= ~PORT_PCR_MUX_MASK;
  PORTA->PCR[LED_R12] |= PORT_PCR_MUX(1);
//  PORTA->PCR[LED_R13] &= ~PORT_PCR_MUX_MASK;
//  PORTA->PCR[LED_R13] |= PORT_PCR_MUX(1);
//  PORTA->PCR[LED_R14] &= ~PORT_PCR_MUX_MASK;
//  PORTA->PCR[LED_R14] |= PORT_PCR_MUX(1);
//  PORTA->PCR[LED_R15] &= ~PORT_PCR_MUX_MASK;
//  PORTA->PCR[LED_R15] |= PORT_PCR_MUX(1);
//  PORTA->PCR[LED_R16] &= ~PORT_PCR_MUX_MASK;
//  PORTA->PCR[LED_R16] |= PORT_PCR_MUX(1);

  // Configure MUX settings for front led
  PORTC->PCR[LED_G3] &= ~PORT_PCR_MUX_MASK;
  PORTC->PCR[LED_G3] |= PORT_PCR_MUX(1);  
  PORTC->PCR[LED_G4] &= ~PORT_PCR_MUX_MASK;
  PORTC->PCR[LED_G4] |= PORT_PCR_MUX(1);
  PORTC->PCR[LED_G5] &= ~PORT_PCR_MUX_MASK;
  PORTC->PCR[LED_G5] |= PORT_PCR_MUX(1);
  PORTC->PCR[LED_G6] &= ~PORT_PCR_MUX_MASK;
  PORTC->PCR[LED_G6] |= PORT_PCR_MUX(1);
  PORTC->PCR[LED_G7] &= ~PORT_PCR_MUX_MASK;
  PORTC->PCR[LED_G7] |= PORT_PCR_MUX(1);
  PORTC->PCR[LED_G8] &= ~PORT_PCR_MUX_MASK;
  PORTC->PCR[LED_G8] |= PORT_PCR_MUX(1);
  PORTC->PCR[LED_G9] &= ~PORT_PCR_MUX_MASK;
  PORTC->PCR[LED_G9] |= PORT_PCR_MUX(1);
  PORTC->PCR[LED_G10] &= ~PORT_PCR_MUX_MASK;
  PORTC->PCR[LED_G10] |= PORT_PCR_MUX(1);
 
  // Set Data Direction Registers for PortA and PortC
  // PTA->PDDR |= (MASK(LED_R3) | MASK(LED_R4) | MASK(LED_R5) | MASK(LED_R12) | MASK(LED_R13) | MASK(LED_R14) | MASK(LED_R15) | MASK(LED_R16));
	PTA->PDDR |= MASK(LED_R12);
  PTC->PDDR |= (MASK(LED_G3) | MASK(LED_G4) | MASK(LED_G5) | MASK(LED_G6) | MASK(LED_G7) | MASK(LED_G8) | MASK(LED_G9) | MASK(LED_G10));
}

void stationaryModeGreen(void)
{
  PTC->PDOR |= (MASK(LED_G3) | MASK(LED_G4) | MASK(LED_G5) | MASK(LED_G6) | MASK(LED_G7) | MASK(LED_G8) | MASK(LED_G9) | MASK(LED_G10));
}
void clearAll(void){
	    PTC->PDOR &= (~MASK(LED_G3) & ~MASK(LED_G4) & ~MASK(LED_G5) & ~MASK(LED_G6) & ~MASK(LED_G7) & ~MASK(LED_G8) & ~MASK(LED_G9) & ~MASK(LED_G10));
}

void stationaryModeRed(void)
{
  PTA -> PDOR |= MASK(LED_R12);
  osDelay(RED_STOP);
  PTA -> PDOR &= ~MASK(LED_R12);
  osDelay(RED_STOP);
}

void runningModeRed(void)
{
  //RED
  PTA -> PDOR |= MASK(LED_R12);
  osDelay(RED_MOVE);
  PTA -> PDOR &= ~MASK(LED_R12);
  osDelay(RED_MOVE);
}

// works properly w normal delay
// how to convert to osDelay??
void runningModeGreen(void)
{
    PTC->PDOR &= (~MASK(LED_G3) & ~MASK(LED_G4) & ~MASK(LED_G5) & ~MASK(LED_G6) & ~MASK(LED_G7) & ~MASK(LED_G8) & ~MASK(LED_G9) & ~MASK(LED_G10));
    for (int i = 0; i < 8; i++) {
			//PTC -> PDOR |= MASK(greenLeds[ledChoice]);
			PTC -> PDOR |= MASK(greenLeds[i]);
			//osDelay(GREEN_MOVE);
			delay(0x8040);
			//PTC -> PDOR &= ~MASK(greenLeds[ledChoice]);
			PTC -> PDOR &= ~MASK(greenLeds[i]);
			//osDelay(GREEN_MOVE);
			delay(0x8040);
    }
}

//int main() {
//	initLED();
//	
//	while (1) {
//		//stationaryModeRed();
//		//delay(0x8000);
//		//runningModeRed();
//		//delay(0x8000);
//		
//		//for (int i = 0 ; i < 3 ; i++) {
//			runningModeGreen();
//		//}
//		
//	}
//}