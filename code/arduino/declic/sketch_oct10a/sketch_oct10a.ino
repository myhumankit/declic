// MHK_Button library
#include <MHK_Button.h>

#define PIN_NUMBER 20

// Button object
Button button[PIN_NUMBER] = {
  Button(A0, HIGH),
  Button(A1, HIGH),
  Button(A2, HIGH),
  Button(A3, HIGH),
  Button(A4, HIGH),
  Button(A5, HIGH),
  Button(0, HIGH),
  Button(1, HIGH),
  Button(2, HIGH),
  Button(3, HIGH),
  Button(4, HIGH),
  Button(5, HIGH),
  Button(6, HIGH),
  Button(7, HIGH),
  Button(8, HIGH),
  Button(9, HIGH),
  Button(10, HIGH),
  Button(11, HIGH),
  Button(12, HIGH),
  Button(13, HIGH)
};


#include <Joystick.h>

Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID, JOYSTICK_TYPE_GAMEPAD,
                   PIN_NUMBER, 0,                  // Button Count, Hat Switch Count
                   true, true, false,     // X and Y, but no Z Axis
                   false, false, false,   // No Rx, Ry, or Rz
                   false, false,          // No rudder or throttle
                   false, false, false);  // No accelerator, brake, or steering




void setup() {
  // initialize serial communications
  //Serial.begin(115200);

  for (int i = 0; i < PIN_NUMBER; i++) {
    // initialize Button object
    button[i].begin(INPUT_PULLUP); // mode: INPUT / INPUT_PULLUP (default: INPUT)

    // configure Button object
    button[i].setDebounceTime(20); // duration in milliseconds: 0 to 255 (default: 0)
    button[i].setHoldTime(2000);   // duration in milliseconds: 0 to 65535 (default: 1000)
    button[i].setIdleTime(3000);   // duration in milliseconds: 0 to  65535 (default: 1000)
    button[i].setMultiTime(255);   // duration in milliseconds: 0 to 255 (default: 127)
  }

  // Initialize Joystick Library
  Joystick.begin();
  Joystick.setXAxisRange(-1, 1);
  Joystick.setYAxisRange(-1, 1);
}

void loop() {

  // update button state
  for (int i = 0; i < PIN_NUMBER; i++) {
    button[i].read();
  };

  for (int i = 0; i < PIN_NUMBER; i++) {
    if (button[i].pressed()) {
      Joystick.setButton(i, 1);
    }
    if (button[i].released()) {
      Joystick.setButton(i, 0);
    }
  };

}
