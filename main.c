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

//motor commands
#define MOVEMENT  0b00000011
#define DIRECTION 0b111100
#define SPEED     0b01000000
#define END 0b10000000

#define MSG_COUNT 2

int isMoving = 0;
int* ptr = &isMoving;

osMessageQueueId_t tAudioMsg, tMotorMsg, tBrainMsg, tGreenMsg, tRedMsg;

//variable to store data received from UART
uint8_t uartData;
uint8_t receivedData;

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
		uint8_t end = command & END;
		if (end == 0b10000000) {
			ending_tune();
		} else {
            background_tune();
        }
	}
}

void tGreen(){
		uint8_t command = NODATA;
	for(;;) {
		//receive mesage and put it into command
		osMessageQueueGet(tGreenMsg, &command, NULL, 0);
		receivedData = command;
		//uint8_t mvmt = command & MOVEMENT;
		if (*ptr == 1){
			runningModeGreen(ptr);
		}
		else{
			stationaryModeGreen();
		}
}
}
void tRed(){
		uint8_t command = NODATA;
	for(;;) {
		//receive mesage and put it into command
		osMessageQueueGet(tRedMsg, &command, NULL, 0);
		receivedData = command;
		//uint8_t mvmt = command & MOVEMENT;
		if (*ptr == 0){
			stationaryModeRed();
		}
		else{
			runningModeRed();
		}
}
}
void tMotor() {
	uint8_t command = NODATA;
	
	for(;;) {
		//receive mesage and put it into command
		osMessageQueueGet(tMotorMsg, &command, NULL, 0);
		receivedData = command;
		uint8_t mvmt = command & MOVEMENT;
		uint8_t degree = (command & DIRECTION) >> 2;
		// for 4 speed (only usable with 6 degree
		//uint8_t velocity = (command & SPEED) >>5;
		
		//for 2 speed 
		uint8_t velocity = (command & SPEED) >>6;
		int left_ratio ;
		int right_ratio ;
		int speed;
		/**
	switch (velocity) {
			case 0b00:
				speed = 40;
				break;
			case 0b01:				
				speed = 60;
				break;		
			case 0b10:
				
				speed = 80;
				break;
			
			case 0b11:
				
				speed = 100;
				break;
			
			default:
				speed = 40;
			break;
		}
		switch (degree) {
			

			case 0b000: 
				left_ratio = 100;
				right_ratio = 100; 

				break;
			
			case 0b001: 
				left_ratio = 60;
				right_ratio = 100; 


				break;
			
			case 0b010: 
				left_ratio = 40;
				right_ratio = 100; 
				break;

			case 0b011: 
				left_ratio = 0;
				right_ratio = 100; 
				break;
			
			
			case 0b100: 
				left_ratio = 100;
				right_ratio = 60; 
				break;
			
			case 0b101:
				left_ratio = 100;
				right_ratio = 40; 
				break;

			case 0b110: 
				left_ratio = 100;
				right_ratio = 0; 
				break;
			
			default:
				left_ratio = 100;
				right_ratio = 100; 
				break;
			
		}**/
		
		//BELOW IS 10DEGREE ABOVE IS 6 DEGREE
		switch (velocity) {
			case 0b0:
				speed = 50;
				break;
			case 0b1:				
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
			
			case 0b0001: 
				left_ratio = 80;
				right_ratio = 100; 

				break;
			
			case 0b0010: 
				left_ratio = 60;
				right_ratio = 100; 

			break;

			case 0b0011: 
				left_ratio = 40;
				right_ratio = 100; 
				break;
			case 0b0100: 
				left_ratio = 20;
				right_ratio = 100; 
				break;
			case 0b0101: 
				left_ratio = 0;
				right_ratio = 100; 
				break;
						
			case 0b0110: 
				left_ratio = 100;
				right_ratio = 80; 
				break;
			
			case 0b0111: 
				left_ratio = 100;
				right_ratio = 60; 
				break;

			case 0b1000: 
				left_ratio = 100;
				right_ratio = 40; 
				break;
			case 0b1001: 
				left_ratio = 100;
				right_ratio = 20; 
				break;
			case 0b1010: 
				left_ratio = 100;
				right_ratio = 0; 
				break;
			
			default:
				left_ratio = 100;
				right_ratio = 100; 
				break;
			
		}
		
		
		
		switch (mvmt) {
			case 0b00:
				//noMove
			//stationaryModeGreen();
			isMoving = 0;
				stopMotors();
				break;
			
			case 0b01:
				//forward
			//runningModeGreen();
			isMoving = 1;
				forward(left_ratio, right_ratio,speed);
				break;
			
			case 0b10:
				//back
			//runningModeGreen();
			isMoving = 1;
				reverse( left_ratio,right_ratio, speed);
				break;
			
			default:
				//noMove
			//stationaryModeGreen();
			isMoving = 0;
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
		uint8_t end = uartData & END;
		if (end == 0b10000000) {
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
	osThreadNew(tGreen, NULL, NULL); 
osThreadNew(tRed, NULL, NULL);     
	
	
	tBrainMsg = osMessageQueueNew(MSG_COUNT, sizeof(uint8_t), NULL);
    tMotorMsg = osMessageQueueNew(MSG_COUNT, sizeof(uint8_t), NULL);
    tAudioMsg = osMessageQueueNew(MSG_COUNT, sizeof(uint8_t), NULL);
	tGreenMsg = osMessageQueueNew(MSG_COUNT, sizeof(uint8_t), NULL);
	tRedMsg = osMessageQueueNew(MSG_COUNT, sizeof(uint8_t), NULL);
		
	osKernelStart();                     
	for (;;) {}
}
