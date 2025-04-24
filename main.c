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
int endAudioPlay = 0;
int isMoving = 0;
int* ptr = &isMoving;
int* ptrAudio =&endAudioPlay;

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
		osMessageQueueGet(tAudioMsg, &command, NULL, 0);
		
		if (*ptrAudio ==1 ) { // checks flag for sending toggling audio
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
		if (*ptr == 1){ // check ptr for triggering led colour
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
		uint8_t end = command & END;
		
		// for 4 speed (only usable with 6 degree
		if(end == END){
			endAudio =1;
			endAudioPlay = 1;
		}
		else{
			
			endAudioPlay = 0;
		}
		
		//for 2 speed 
		uint8_t velocity = (command & SPEED) >>6;
		int left_ratio ;
		int right_ratio ;
		int left_inverse_ratio;
		int right_inverse_ratio;
		int speed;
		
		
		//recieving velocity bits and relating them to speed ratio
		switch (velocity) {
			case 0b0:
				speed = 65;
				break;
			case 0b1:				
				speed = 100;
				break;		
			default:
				speed = 65;
			break;
		}
		
		// recieving degree bits and converting them into ratio of each motor
		switch (degree) {
			

			case 0b0000: 
				left_ratio = 100;
				right_ratio = 100;
				left_inverse_ratio =0;
				right_inverse_ratio = 0;
			

				break;
			
			case 0b0001: 
				left_ratio = 20;
				right_ratio = 100; 
		  	left_inverse_ratio =0;
				right_inverse_ratio = 0;

				break;
			
			case 0b0010: 
				left_ratio = 10;
				right_ratio = 100; 
		  	left_inverse_ratio =0;
				right_inverse_ratio = 0;

			break;

			case 0b0011: 
				left_ratio = 0;
				right_ratio = 100; 
	  		left_inverse_ratio =0;
				right_inverse_ratio = 0;
				break;
			case 0b0100: 
				left_ratio = 0;
				right_ratio = 100; 
		  	left_inverse_ratio =10;
				right_inverse_ratio = 0;
				break;
			case 0b0101: 
				left_ratio = 0;
				right_ratio = 100; 
				left_inverse_ratio = 30;
				right_inverse_ratio = 0;
				break;
						
			case 0b0110: 
				left_ratio = 100;
				right_ratio = 20; 
				left_inverse_ratio = 0;
				right_inverse_ratio = 0;
				break;
			
			case 0b0111: 
				left_ratio = 100;
				right_ratio = 10; 
				left_inverse_ratio = 0;
				right_inverse_ratio = 0;
				break;

			case 0b1000: 
				left_ratio = 100;
				right_ratio = 0; 
				left_inverse_ratio = 0;
				right_inverse_ratio = 0;
				break;
			case 0b1001: 
				left_ratio = 100;
				right_ratio = 0; 
				left_inverse_ratio = 0;
				right_inverse_ratio = 10;
				break;
			case 0b1010: 
				left_ratio = 100;
				right_ratio = 0; 
			left_inverse_ratio = 0;
				right_inverse_ratio = 30;
				break;
			
			default:
				left_ratio = 100;
				right_ratio = 100; 
				break;
			
		}
		
		
				
		
		
		// call each movement 
		switch (mvmt) {
			case 0b00:
				//noMove
			isMoving = 0;
				stopMotors();
				break;
			
			case 0b01:
				//forward
			isMoving = 1;
				forward(left_ratio, right_ratio,speed,left_inverse_ratio,right_inverse_ratio);
				break;
			
			case 0b10:
				//back
			isMoving = 1;
				reverse( left_ratio,right_ratio, speed,left_inverse_ratio,right_inverse_ratio);
				break;
			
			default:
				//noMove
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
		//uint8_t end = uartData & END;
		//if (end == 0b10000000) {
			osMessageQueuePut(tAudioMsg, &uartData, NULL, 0);
		//}
		
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