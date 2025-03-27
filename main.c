#include "MKL25Z4.h"                    
#include "RTE_Components.h"
#include  CMSIS_device_header
#include "cmsis_os2.h"
#include "Serial_ISR.h"
#include "audio.h"
#include "led.h"
#include "motor.h"

//null command
#define NODATA 0x00
#define END_CHALLENGE 0x37

//motor commands
#define MOVEMENT 0b11
#define DIRECTION 0b11100
#define SPEED 0b1100000

#define MSG_COUNT 2

osMessageQueueId_t tAudioMsg, tMotorMsg, tGreenMsg, tRedMsg, tBrainMsg;

//variable to store data received from UART
uint8_t uartData;

//UART2 Interrupt Request Handler
void UART2_IRQHandler(void) 
{
	if (UART2->S1 & UART_S1_RDRF_MASK) {
		rx_data = UART2->D;
		osMessageQueuePut(tBrainMsg, &rx_data, NULL, 0);
	}
}

void tAudio() {
	uint8_t command = NODATA;
	for(;;) {
		//receive mesage and put it into command
		osMessageQueueGet(tAudioMsg, &command, NULL, osWaitForever);

		if (command == END_CHALLENGE) {
			ending_tune();
		} else {
            background_tune();
        }
	}
}

void tMotor() {
	uint8_t command = NODATA;
	for(;;) {
		//receive mesage and put it into command
		osMessageQueueGet(tMotorMsg, &command, NULL, 0);
		uint8_t mvmt = command & MOVEMENT;
		uint8_t degree = (command & DIRECTION)>>2 ;
		uint8_t velocity = (command & SPEED) >>5;
		uint8_t left_ratio ;
		uint8_t right_ratio ;
		uint8_t speed;
		
	switch (velocity) {
			case 0b00:
				speed = 50;
				break;
			case 0b01:				
				speed = 70;
				break;		
			case 0b10:
				
				speed = 80;
				break;
			
			case 0b11:
				
				speed = 100;
				break;
			
			default:
				speed = 50;
			break;
		}
		
		switch (degree) {
			

			case 0b0000: 
				left_ratio = 100;
				right_ratio = 100; 

				break;
			
			case 0b000001: 
				left_ratio = 80;
				right_ratio = 100; 


				break;
			
			case 0b000010: 
				left_ratio = 40;
				right_ratio = 100; 
				break;

			case 0b000111: 
				left_ratio = 0;
				right_ratio = 100; 
				break;
			
			
			case 0b100: 
				left_ratio = 100;
				right_ratio = 80; 
				break;
			
			case 0b101:
				left_ratio = 100;
				right_ratio = 40; 
				break;

			case 0b110: 
				left_ratio = 100;
				right_ratio = 10; 
				break;
			
			default:
				left_ratio = 100;
				right_ratio = 100; 
				break;
			
		}
		
		
		
		switch (mvmt) {
			case 0b00:
				//noMove
				stopMotors();
				break;
			
			case 0b01:
				//forward
				forward(left_ratio, right_ratio,speed);
				break;
			
			case 0b10:
				//back
				reverse( left_ratio,right_ratio, speed);
				break;
			
			default:
				//noMove
				stopMotors();
		}

		
	}
}


void tBrain() {
	for(;;) {
		//get message from UART IRQ and put it in uartData for every other thread to access
		osMessageQueueGet(tBrainMsg, &uartData, NULL, osWaitForever);
		//send uartData to corresponding thread
		osMessageQueuePut(tMotorMsg, &uartData, NULL, 0);
		
		if (uartData == END_CHALLENGE) {
			osMessageQueuePut(tAudioMsg, &uartData, NULL, 0);
		}
		
        osMessageQueuePut(tGreenMsg, &uartData, NULL, 0);
		osMessageQueuePut(tRedMsg, &uartData, NULL, 0);
	}
}

int main (void) {
  // System Initialization
    SystemCoreClockUpdate();
	
	//Enable board LED
	initLED();
	
	//Enable Audio
	initAudio();
	
	//Enable UART
	initUART2(BAUD_RATE);
	//Enable motor
	initMotors();
 
	osKernelInitialize();            

	osThreadNew(tBrain, NULL, NULL);    
    osThreadNew(tMotor, NULL, NULL);   
    osThreadNew(tAudio, NULL, NULL);    
	
	
	tBrainMsg = osMessageQueueNew(MSG_COUNT, sizeof(uint8_t), NULL);
    tMotorMsg = osMessageQueueNew(MSG_COUNT, sizeof(uint8_t), NULL);
    tAudioMsg = osMessageQueueNew(MSG_COUNT, sizeof(uint8_t), NULL);
    tGreenMsg = osMessageQueueNew(MSG_COUNT, sizeof(uint8_t), NULL);
	tRedMsg = osMessageQueueNew(MSG_COUNT, sizeof(uint8_t), NULL);
		
	osKernelStart();                     
	for (;;) {}
}
