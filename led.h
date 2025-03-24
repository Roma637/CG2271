// Rear Red LED Pin Config
#define LED_R12 12 // PortA Pin 12

// Front Green LED Pin Config
#define LED_G3 3 // PortC Pin 3
#define LED_G4 4 // PortC Pin 4
#define LED_G5 5 // PortC Pin 5
#define LED_G6 6 // PortC Pin 6
#define LED_G7 7 // PortC Pin 7
#define LED_G8 8 // PortC Pin 8
#define LED_G9 9 // PortC Pin 9
#define LED_G10 10 // PortC Pin 10
#define MASK(x) (1 << (x))

// LED SPEEDS
#define RED_STOP 250 // milliseconds
#define RED_MOVE 500 //milliseconds
#define GREEN_FLASH 300 //milliseconds
#define GREEN_MOVE 250 //milliseconds

void greenFlash(void);
void initLED(void);
void runningModeRed(void);
void stationaryModeRed(void);
void runningModeGreen(void);
void stationaryModeGreen(void);
