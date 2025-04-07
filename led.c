#include "MKL25Z4.h"                    // Device header
#include "RTE_Components.h"
#include CMSIS_device_header
#include "cmsis_os2.h"                  // ::CMSIS:RTOS2
#include "led.h"

uint8_t greenLeds[8] = {LED_G3, LED_G4, LED_G5, LED_G6, LED_G7, LED_G8, LED_G9, LED_G10};


static void delay(volatile uint32_t nof) {
  while(nof!=0) {
    __asm("NOP");
    nof--;
  }
}

void initLED(void) {
  SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;


  PORTC->PCR[LED_R2] &= ~PORT_PCR_MUX_MASK;
  PORTC->PCR[LED_R2] |= PORT_PCR_MUX(1); // ALT4 => TPM0_CH1
  PTC->PDDR |= MASK(LED_R2);
  
  for (int i = 0; i < 8; i++) {
      PORTC->PCR[greenLeds[i]] &= ~PORT_PCR_MUX_MASK;
      PORTC->PCR[greenLeds[i]] |= PORT_PCR_MUX(1);  // GPIO mode
      PTC->PDDR |= MASK(greenLeds[i]);            // Set pin as output
    //  PTC->PSOR = MASK(greenLeds[i]);
  }
  
}


void stationaryModeGreen(void)
{
  PTC->PDOR |= (MASK(LED_G3) | MASK(LED_G4) | MASK(LED_G5) | MASK(LED_G6) | MASK(LED_G7) | MASK(LED_G8) | MASK(LED_G9) | MASK(LED_G10));
}

void stationaryModeRed(void)
{
  PTC -> PDOR |= MASK(LED_R2);
  osDelay(RED_STOP);
  PTC -> PDOR &= ~MASK(LED_R2);
  osDelay(RED_STOP);
}

void runningModeRed(void)
{
  PTC -> PDOR |= MASK(LED_R2);
  osDelay(RED_MOVE);
  PTC -> PDOR &= ~MASK(LED_R2);
  osDelay(RED_MOVE);
}

// works properly w normal delay
// how to convert to osDelay??
void runningModeGreen(int* ptr) {
  for (int i = 0; i < 8; i++) {
		if(*ptr == 1){      
			PTC->PSOR = MASK(greenLeds[i]);  
      osDelay(GREEN_MOVE);
      
      //turn LED off
      //PTC->PCOR = MASK(greenLeds[i]);
      //osDelay(GREEN_MOVE);
		PTC->PCOR = (MASK(LED_G3) | MASK(LED_G4) | MASK(LED_G5) | MASK(LED_G6) | MASK(LED_G7) | MASK(LED_G8) | MASK(LED_G9) | MASK(LED_G10));
      osDelay(GREEN_MOVE);
		}

  }
}

