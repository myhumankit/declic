#include <Mouse.h>
#include <MHK_Button.h>

Button bouton(1, HIGH);

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  Mouse.begin();
  bouton.begin(INPUT_PULLUP);

  // configure Button object
  bouton.setDebounceTime(50); // duration in milliseconds: 0 to 255 (default: 0)
  bouton.setHoldTime(1500);   // duration in milliseconds: 0 to 65535 (default: 1000)
  bouton.setIdleTime(3000);   // duration in milliseconds: 0 to  65535 (default: 1000)
  bouton.setMultiTime(255);   // duration in milliseconds: 0 to 255 (default: 127)

}

unsigned long new_click = 0;
unsigned long  click_delay = 0;
bool is_hold = false;

void smartRelease(unsigned long measured_delay) {
  if (measured_delay < 500) {
    Mouse.click();
  }
  else if (measured_delay < 1500) {
    Mouse.press();
    is_hold = true;
  }
}

void loop()
{
  // put your main code here, to run repeatedly:
  int axe_x = analogRead(A1);
  int axe_y = analogRead(A0);

  int sensibility = analogRead(A2);
  int sensibility_range = map(sensibility, 0, 1023, 1, 50);

  int pixel_x = map(axe_x, 0, 1023, -sensibility_range, sensibility_range);
  int pixel_y = map(axe_y, 0, 1023, sensibility_range, -sensibility_range);

  //Serial.print(pixel_x);
  //Serial.print(" ");
  //Serial.println(pixel_y);
  Mouse.move(pixel_x, pixel_y, 0);

  bouton.read(); // update button state

  if (bouton.pressed())
  {
    if (!is_hold) {
      new_click = millis();
    }
    else {
      Mouse.release();
      is_hold = false;
    }
  }

  if (bouton.released())
  {
    click_delay = millis() - new_click;
    smartRelease(click_delay);
  }

  if (bouton.hold())
  {
    Mouse.click(MOUSE_RIGHT);
  }
  delay(5);
}
