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
#define MOVE_FORWARD 0x80
#define TURN_LEFT 0x41
#define TURN_RIGHT 0x42
#define MOVE_BACKWARD 0x43
#define MOVE_FORWARD_LEFT 0x44
#define MOVE_FORWARD_RIGHT 0x45
#define MOVE_BACKWARD_LEFT 0x46
#define MOVE_BACKWARD_RIGHT 0x47
#define STOP 0x48
#define MOVEMENT 0b11
#define DIRECTION 0b11100

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
		
		uint8_t degree = (command & DIRECTION)>>2 ;
		
		uint8_t left_ratio ;
		uint8_t right_ratio ;
		
		switch (degree) {
			
//		000 center
//		001 left D1
//		010 left d2
//		011 left d3
			
			case 0b000: 
				left_ratio = 50;
				right_ratio = 50; 
				break;
			
			case 0b001: 
				left_ratio = 40;
				right_ratio = 60; 
				break;
			
			case 0b010: 
				left_ratio = 30;
				right_ratio = 70; 
				break;

			case 0b011: 
				left_ratio = 20;
				right_ratio = 80; 
				break;
			
//		100 right d1
//		101 right d2
//		110 right d3
			
			case 0b100: 
				left_ratio = 60;
				right_ratio = 40; 
				break;
			
			case 0b101: 
				left_ratio = 70;
				right_ratio = 30; 
				break;

			case 0b110: 
				left_ratio = 80;
				right_ratio = 20; 
				break;
			
			default:
				left_ratio = 50;
				right_ratio = 50; 
				break;
			
		}
		
		uint8_t mvmt = command & MOVEMENT;
		//uint
		
		switch (mvmt) {
			case 0b00:
				//noMove
				stopMotors();
				break;
			
			case 0b01:
				//forward
				forward(left_ratio, right_ratio);
				break;
			
			case 0b10:
				//back
				reverse(MOTOR_DEBUG_SPEED);
				break;
			
			default:
				//noMove
				stopMotors();
		}
//		if() {
//			forward(MOTOR_DEBUG_SPEED);
//		} else if (command == TURN_LEFT) {
//			left(MOTOR_DEBUG_SPEED);
//		} else if (command == TURN_RIGHT) {
//			right(MOTOR_DEBUG_SPEED);
//		} else if (command%2==0) {
//			reverse(MOTOR_DEBUG_SPEED);
//		} else if (command == MOVE_FORWARD_LEFT) {
//			leftforward();
//		} else if (command == MOVE_FORWARD_RIGHT) {
//			rightforward();
//		} else if (command == MOVE_BACKWARD_LEFT) {
//			leftreverse();
//		} else if (command == MOVE_BACKWARD_RIGHT) {
//			rightreverse();
//		} else { 
//			stopMotors();
//		}
	}
}

void tGreen() {
	uint8_t command = NODATA;
	
	for(;;) {
		osMessageQueueGet(tGreenMsg, &command, NULL, 0);

		if (command == STOP || command == END_CHALLENGE) {     // If robot is stationary
			stationaryModeGreen();
		} else if (command == MOVE_FORWARD || command == MOVE_BACKWARD || command == MOVE_FORWARD_LEFT
			|| command == MOVE_FORWARD_RIGHT || command == MOVE_BACKWARD_LEFT || command == MOVE_BACKWARD_RIGHT
			|| command == TURN_LEFT || command == TURN_RIGHT){
			runningModeGreen();
		}			
	}
}

void tRed() {
	uint8_t command = NODATA;
	
	for(;;) {
		osMessageQueueGet(tRedMsg, &command, NULL, 0);

		if (command == STOP || command == END_CHALLENGE) {     // If robot is stationary
			stationaryModeRed();
			//red flash on and off with period 0.5sec
		} else if (command == MOVE_FORWARD || command == MOVE_BACKWARD || command == MOVE_FORWARD_LEFT
			|| command == MOVE_FORWARD_RIGHT || command == MOVE_BACKWARD_LEFT || command == MOVE_BACKWARD_RIGHT
			|| command == TURN_LEFT || command == TURN_RIGHT){
			runningModeRed();
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
