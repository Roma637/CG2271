#include "MKL25Z4.h"                    // Device header

#define UART_TX_PORTE22 22
#define UART_RX_PORTE23 23
#define BAUD_RATE 9600
#define UART2_INT_PRIO 128

extern uint8_t rx_data;
extern uint8_t endAudio;


void initUART2(uint32_t baud_rate);
//void UART2_IRQHandler(void);
void UART2_Transmit_Poll(uint8_t data);
