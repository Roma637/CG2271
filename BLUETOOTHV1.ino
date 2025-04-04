#include <Bluepad32.h>
#include <stdarg.h>
#include <math.h>


#define UART_BAUD_RATE 9600
#define SERIAL_BAUD_RATE 115200



// button definitions
#define A_BUTTON 0x0002
#define B_BUTTON 0x0001
#define Y_BUTTON 0x0004
#define X_BUTTON 0x0008
//L-R boundaries
#define LEFT_MAX -500
#define LEFT_MIN -150
#define RIGHT_MAX 510
#define RIGHT_MIN 150

// Speed Mode Definitions
#define SPEED_MODE_1      (0b0 << 6)// 00 in bits -4
#define SPEED_MODE_2      (0b1 << 6)// 01 in bits 5-4



#define GAME_OVER 0b10000000

// Direction Definitions
#define DIRECTION_FORWARD  0b01  // 01 in bits 1-0
#define DIRECTION_REVERSE  0b10  // 10 in bits 1-0

ControllerPtr myControllers[BP32_MAX_GAMEPADS];
uint8_t message;
int speedVal = 0;
int x_flag = 0;
int x_count = 0;
int y_flag = 0;
int y_count = 0;

// This callback gets called any time a new gamepad is connected.
// Up to 4 gamepads can be connected at the same time.
void onConnectedController(ControllerPtr ctl) {
  bool foundEmptySlot = false;
  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (myControllers[i] == nullptr) {
      Serial.printf("CALLBACK: Controller is connected, index=%d\n", i);
      // Additionally, you can get certain gamepad properties like:
      // Model, VID, PID, BTAddr, flags, etc.
      ControllerProperties properties = ctl->getProperties();
      Serial.printf("Controller model: %s, VID=0x%04x, PID=0x%04x\n", ctl->getModelName().c_str(), properties.vendor_id,
                    properties.product_id);
      myControllers[i] = ctl;
      foundEmptySlot = true;
      break;
    }
  }
  if (!foundEmptySlot) {
    Serial.println("CALLBACK: Controller connected, but could not found empty slot");
  }
}

void onDisconnectedController(ControllerPtr ctl) {
  bool foundController = false;

  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (myControllers[i] == ctl) {
      Serial.printf("CALLBACK: Controller disconnected from index=%d\n", i);
      myControllers[i] = nullptr;
      foundController = true;
      break;
    }
  }

  if (!foundController) {
    Serial.println("CALLBACK: Controller disconnected, but not found in myControllers");
  }
}

void dumpGamepad(ControllerPtr ctl) {
  Serial.printf(
    "idx=%d, dpad: 0x%02x, buttons: 0x%04x, axis L: %4d, %4d, axis R: %4d, %4d, brake: %4d, throttle: %4d, "
    "misc: 0x%02x, gyro x:%6d y:%6d z:%6d, accel x:%6d y:%6d z:%6d\n",
    ctl->index(),        // Controller Index
    ctl->dpad(),         // D-pad
    ctl->buttons(),      // bitmask of pressed buttons
    ctl->axisX(),        // (-511 - 512) left X Axis
    ctl->axisY(),        // (-511 - 512) left Y axis
    ctl->axisRX(),       // (-511 - 512) right X axis
    ctl->axisRY(),       // (-511 - 512) right Y axis
    ctl->brake(),        // (0 - 1023): brake button
    ctl->throttle(),     // (0 - 1023): throttle (AKA gas) button
    ctl->miscButtons(),  // bitmask of pressed "misc" buttons
    ctl->gyroX(),        // Gyro X
    ctl->gyroY(),        // Gyro Y
    ctl->gyroZ(),        // Gyro Z
    ctl->accelX(),       // Accelerometer X
    ctl->accelY(),       // Accelerometer Y
    ctl->accelZ()        // Accelerometer Z
  );
}

void callStop() {
  message &= ~(0b11);
}
void callForward() {
  message &= ~(0b11);
  message |= DIRECTION_FORWARD;
}
void callBackward() {
  message &= ~(0b11);
  message |= DIRECTION_REVERSE;
}
void callCentre() {
  message &= ~(0b1111 << 2);
}
void callLeftD1() {
  message &= ~(0b1111 << 2);
  message |= (1 << 2);
}
void callLeftD2() {
  message &= ~(0b1111 << 2);
  message |= (2 << 2);
}
void callLeftD3() {
  message &= ~(0b1111 << 2);
  message |= (3 << 2);
}
void callLeftD4() {
  message &= ~(0b1111 << 2);
  message |= (4 << 2);
}
void callLeftD5() {
  message &= ~(0b1111 << 2);
  message |= (5 << 2);
}
void callRightD1() {
  message &= ~(0b1111 << 2);
  message |= (6 << 2);
}
void callRightD2() {
  message &= ~(0b1111 << 2);
  message |= (7 << 2);
}
void callRightD3() {
  message &= ~(0b1111 << 2);
  message |= (8 << 2);
}
void callRightD4() {
  message &= ~(0b1111 << 2);
  message |= (9 << 2);
}
void callRightD5() {
  message &= ~(0b1111 << 2);
  message |= (10 << 2);
}
void callGameOver() {

  message ^= GAME_OVER;

}
void setSpeedMessage() {
  message ^= SPEED_MODE_2;


}
void x_debounce() {
  x_count += 1;
  if (x_count >= 35) {
    x_flag = 0;
    x_count = 0;
  }
}
void y_debounce() {
  y_count += 1;
  if (y_count >= 70) {
    y_flag = 0;
    y_count = 0;
  }
}

void processGamepad(ControllerPtr ctl) {
  // There are different ways to query whether a button is pressed.
  // By query each button individually:
  //  a(), b(), x(), y(), l1(), etc...
  int rightRange = RIGHT_MAX - RIGHT_MIN;
  int leftRange = LEFT_MIN - LEFT_MAX;
  int rightUnit = rightRange / 5;
  int leftUnit = leftRange / 5;
  int rightD2 = rightUnit + RIGHT_MIN;
  int rightD3 = rightUnit * 2 + RIGHT_MIN;
  int rightD4 = rightUnit * 3 + RIGHT_MIN;
  int rightD5 = rightUnit * 4 + RIGHT_MIN;
  int leftD2 =  LEFT_MIN - leftUnit;
  int leftD3 = LEFT_MIN - leftUnit * 2;
  int leftD4 = LEFT_MIN - leftUnit * 3;
  int leftD5 = LEFT_MIN - leftUnit * 4;


  x_debounce();
  y_debounce();
  //



  if (ctl->buttons() != BUTTON_A && ctl->buttons() != BUTTON_B ) {
    callStop();
  }
  else if (ctl->buttons() == A_BUTTON) {
    //    Serial.println("A");
    callForward();
  }
  else if (ctl->buttons() == B_BUTTON) {
    //    Serial.println("B");
    callBackward();
  }
  //LEFT
  if ( ctl->axisX() > LEFT_MIN && ctl->axisX() < RIGHT_MIN ) {
    callCentre();
  }
  else if (ctl->axisX() <= LEFT_MIN && ctl->axisX() > leftD2) {
    callLeftD1();
  }
  else if (ctl->axisX() <= leftD2 && ctl->axisX() > leftD3) {
    callLeftD2();
  }
  else if (ctl->axisX() <= leftD3 && ctl->axisX() > leftD4) {
    callLeftD3();
  }
  else if (ctl->axisX() <= leftD4 && ctl->axisX() > leftD5) {
    callLeftD4();
  }
  else if (ctl->axisX() <= leftD5) {
    callLeftD5();
  }
  //RIGHT
  else if (ctl->axisX() > RIGHT_MIN && ctl->axisX() < rightD2) {
    callRightD1();
  }
  else if (ctl->axisX() >= rightD2 && ctl->axisX() < rightD3) {
    callRightD2();
  }
  else if (ctl->axisX() >= rightD3  && ctl->axisX() < rightD4) {
    callRightD3();
  }
  else if (ctl->axisX() >= rightD4 && ctl->axisX() < rightD5) {
    callRightD4();
  }
  else if (ctl->axisX() >= rightD5) {
    callRightD5();
  }
  //GAME
  if (ctl->buttons() == Y_BUTTON) {
    if (!y_flag) {
      y_flag = 1;
      callGameOver();
    }
  }
  //SPEED
  if (ctl->buttons() == X_BUTTON) {
    if (!x_flag) {
      x_flag = 1;
      setSpeedMessage();

    }
  }

  //  dumpGamepad(ctl);
  Serial2.write(message);
  Serial.println(message, BIN);
}


void processControllers() {
  for (auto myController : myControllers) {
    if (myController && myController->isConnected() && myController->hasData()) {
      if (myController->isGamepad()) {
        processGamepad(myController);
      } else {
        Serial.println("Unsupported controller");
      }
    }
  }
}

// Arduino setup function. Runs in CPU 1
void setup() {
  message = 0;
  Serial.begin(SERIAL_BAUD_RATE);
  Serial2.begin(UART_BAUD_RATE);
  Serial.printf("Firmware: %s\n", BP32.firmwareVersion());
  const uint8_t* addr = BP32.localBdAddress();
  Serial.printf("BD Addr: %2X:%2X:%2X:%2X:%2X:%2X\n", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
  pinMode(13, OUTPUT);

  // Setup the Bluepad32 callbacks
  BP32.setup(&onConnectedController, &onDisconnectedController);

  // "forgetBluetoothKeys()" should be called when the user performs
  // a "device factory reset", or similar.
  // Calling "forgetBluetoothKeys" in setup() just as an example.
  // Forgetting Bluetooth keys prevents "paired" gamepads to reconnect.
  // But it might also fix some connection / re-connection issues.
  BP32.forgetBluetoothKeys();

  // Enables mouse / touchpad support for gamepads that support them.
  // When enabled, controllers like DualSense and DualShock4 generate two connected devices:
  // - First one: the gamepad
  // - Second one, which is a "virtual device", is a mouse.
  // By default, it is disabled.
  BP32.enableVirtualDevice(false);
}

// Arduino loop function. Runs in CPU 1.
void loop() {
  // This call fetches all the controllers' data.
  // Call this function in your main loop.
  bool dataUpdated = BP32.update();
  if (dataUpdated)
    processControllers();

  // The main loop must have some kind of "yield to lower priority task" event.
  // Otherwise, the watchdog will get triggered.
  // If your main loop doesn't have one, just add a simple `vTaskDelay(1)`.
  // Detailed info here:
  // https://stackoverflow.com/questions/66278271/task-watchdog-got-triggered-the-tasks-did-not-reset-the-watchdog-in-time

  //    vTaskDelay(1);

  // serialise and send direction every few millseconds



  delay(10);
}
