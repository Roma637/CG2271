
#define LEFT_FW 0 // PTB0 TPM1_CH0
#define LEFT_BK 1 // PTB1 TPM1_CH1
#define RIGHT_FW 2 // PTB2 TPM2_CH0
#define RIGHT_BK 3 // PTB3 TPM2_CH1

#define MOTOR_DEBUG_SPEED 4

//Set modulo value 48000000 / 128 = 375000 / 7500 = 50Hz //128 is PS; 
//when count reach value of MOD_VAL, signal goes back to 0 and counts back up again
#define MOD_VAL 7500
#define SLOWER 0.05
#define SDDC 0.45

void initMotors(void);
void stopMotors(void);
void forward(int left_ratio, int right_ratio,int speed);
void reverse(int left_ratio, int right_ratio,int speed);

