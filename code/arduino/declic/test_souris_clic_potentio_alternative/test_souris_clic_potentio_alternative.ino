#include <Mouse.h>
#include <MHK_Button.h>

Button bouton(1, HIGH);

#define HOLD_RIGHT_TIME 500
unsigned long start_time = 0;
bool is_hold = 0;
bool is_long_press = 0;

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  Mouse.begin();
  bouton.begin(INPUT_PULLUP);

  // configure Button object
  bouton.setDebounceTime(50); // duration in milliseconds: 0 to 255 (default: 0)
  bouton.setHoldTime(500);   // duration in milliseconds: 0 to 65535 (default: 1000)
  bouton.setIdleTime(3000);   // duration in milliseconds: 0 to  65535 (default: 1000)
  bouton.setMultiTime(255);   // duration in milliseconds: 0 to 255 (default: 127)

}

void loop()
{
  // put your main code here, to run repeatedly:
  int axe_x = analogRead(A1);
  int axe_y = analogRead(A0);

  int sensibility = analogRead(A2);
  int sensibility_range = map(sensibility, 0, 1023, 5, 20);

  int pixel_x = map(axe_x, 0, 1023, -sensibility_range, sensibility_range);
  int pixel_y = map(axe_y, 0, 1023, sensibility_range, -sensibility_range);

  //Serial.print(pixel_x);
  //Serial.print(" ");
  //Serial.println(pixel_y);
  Mouse.move(pixel_x, pixel_y, 0);

  bouton.read(); // update button state

  if (bouton.hold()) {
    start_time = millis();
    is_hold = 1;
  }

  if (is_hold and (millis() > HOLD_RIGHT_TIME + start_time)) {
    Mouse.press();
    is_hold = 0;
    is_long_press = 1;
  }

  if (bouton.released()) {
    if (is_hold) {
      if (millis() < HOLD_RIGHT_TIME + start_time) {
        Mouse.click(MOUSE_RIGHT);
      }
      is_hold = 0;
    }
    else {
      if (not is_long_press) {
        Mouse.click();
      }
    }
  }

  if (bouton.pressed() and is_long_press) {
    is_long_press = 0;
    Mouse.release();
  }

  delay(10);
}
