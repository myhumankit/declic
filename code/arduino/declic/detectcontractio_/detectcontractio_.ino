#include <Mouse.h>
#include <MHK_Button.h>

VirtualButton bouton(LOW);

#define HOLD_RIGHT_TIME 500
#define SEUIL 50
unsigned long start_time = 0;
bool is_hold = 0;
bool is_long_press = 0;
bool long_press_end = 0;
int seuil = 400;

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  Mouse.begin();
  // bouton.begin(INPUT_PULLUP);
  // suppression du begin.

  // configure Button object
  //  bouton.setDebounceTime(50); // duration in milliseconds: 0 to 255 (default: 0)
  bouton.setHoldTime(500);   // duration in milliseconds: 0 to 65535 (default: 1000)
  bouton.setIdleTime(3000);   // duration in milliseconds: 0 to  65535 (default: 1000)
  bouton.setMultiTime(255);   // duration in milliseconds: 0 to 255 (default: 127)

}


void loop() {
  // put your main code here, to run repeatedly:
  int axe_x = analogRead(A1);
  int axe_y = analogRead(A0);

  if ((axe_x < (512 + SEUIL)) and (axe_x > (512 - SEUIL))) {
    axe_x = 512;
  }

  if ((axe_y < (512 + SEUIL)) and (axe_y > (512 - SEUIL))) {
    axe_y = 512;
  }

  int sensibility = analogRead(A2);
  int sensibility_range = map(sensibility, 0, 1023, 5, 20);

  int pixel_x = map(axe_x, 0, 1023, -sensibility_range, sensibility_range);
  int pixel_y = map(axe_y, 0, 1023, sensibility_range, -sensibility_range);



  //Serial.print(pixel_x);
  //Serial.print(" ");
  //Serial.println(pixel_y);
  Mouse.move(pixel_x, pixel_y, 0);



  int contraction = analogRead(A5);
  Serial.println(contraction);



  //bouton.read(); // update button state
  // remplacement de bouton read par boutonset.
  // boutonset vaut 1 si la contraction est détecté , vaut 0 sinon.

  if (contraction > seuil) {

    bouton.set (1);
  }
  else {
    bouton.set (0);
  }


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
      if ((not is_long_press) and (not long_press_end)) {
        Mouse.click();
      }
    }
    long_press_end = 0;
  }

  if (bouton.pressed() and is_long_press) {
    is_long_press = 0;
    long_press_end = 1;
    Mouse.release();

  }

  delay(10);
}
