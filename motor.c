#include "MKL25Z4.h"
#include "motor.h"

// Motor Initialization (Call this once)
void initMotors() {
    // Enable clock for PORTB and TPM1
    SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;
    SIM->SCGC6 |= (SIM_SCGC6_TPM1_MASK | SIM_SCGC6_TPM2_MASK);

    // Configure mode 3 for PWM operation
    PORTB->PCR[LEFT_FW] &= ~PORT_PCR_MUX_MASK;
    PORTB->PCR[LEFT_FW] |= PORT_PCR_MUX(3);
    PORTB->PCR[LEFT_BK] &= ~PORT_PCR_MUX_MASK;
    PORTB->PCR[LEFT_BK] |= PORT_PCR_MUX(3);
    
    PORTB->PCR[RIGHT_FW] &= ~PORT_PCR_MUX_MASK;
    PORTB->PCR[RIGHT_FW] |= PORT_PCR_MUX(3);
    PORTB->PCR[RIGHT_BK] &= ~PORT_PCR_MUX_MASK;
    PORTB->PCR[RIGHT_BK] |= PORT_PCR_MUX(3);

    // Set Clock Source 48MHz
    SIM->SOPT2 &= ~SIM_SOPT2_TPMSRC_MASK; 
    SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);

    // Set Mode and Prescaler
    TPM1->MOD = MOD_VAL;
    TPM2->MOD = MOD_VAL;

    TPM1->SC &= ~((TPM_SC_CMOD_MASK) | (TPM_SC_PS_MASK));
    TPM1->SC |= (TPM_SC_CMOD(1) | TPM_SC_PS(7));
    TPM1->SC &= ~(TPM_SC_CPWMS_MASK);

    TPM2->SC &= ~((TPM_SC_CMOD_MASK) | (TPM_SC_PS_MASK));
    TPM2->SC |= (TPM_SC_CMOD(1) | TPM_SC_PS(7));
    TPM2->SC &= ~(TPM_SC_CPWMS_MASK);

    //Enable PWM on TPM1 CHannel 0 -> PTB0
	TPM1_C0SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK)); //all this for setting bits
	TPM1_C0SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1)); //setting elsb, mab to 1 and everything else to 0 //see pg555
	
	//Enable PWM on TPM1 Channel 1 -> PTB1
	TPM1_C1SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
	TPM1_C1SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
	
	//Enable PWM on TPM2 channel 0 -> PTB2
	TPM2_C0SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
	TPM2_C0SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
	
	//Enable PWM on TPM2 channel 1 -> PTB3
	TPM2_C1SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
	TPM2_C1SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
}

void stopMotors() {
	TPM1->MOD = 0;
	TPM1_C0V = 0; //stop left fw
	TPM1_C1V = 0; //stop left bk
	
	TPM2->MOD = 0;
	TPM2_C0V = 0; //stop right fw
	TPM2_C1V = 0; //stop right bk
}

void forward(int left_ratio, int right_ratio, int speed) { //speed can be retrieved from UART and broken down from 0 to 1 to represent duty cycle??
  // left_ratio and right_ratio range 0-100 
		TPM1->MOD = MOD_VAL;
    TPM1_C0V = (int) (MOD_VAL * right_ratio/100)*speed/100;
    TPM1_C1V = 0;
      
    TPM2->MOD = MOD_VAL;
    TPM2_C0V = (int) (MOD_VAL * left_ratio/100)*speed/100;
    TPM2_C1V = 0;
}

void reverse(int left_ratio, int right_ratio,int speed) {
    TPM1->MOD = MOD_VAL;
    TPM1_C0V = 0;
    TPM1_C1V = (int) (MOD_VAL * right_ratio /100)*speed/100;
      
    TPM2->MOD = MOD_VAL;
    TPM2_C0V = 0;
    TPM2_C1V = (int) (MOD_VAL * left_ratio /100)*speed/100;
}



