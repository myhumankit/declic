#include <Mouse.h>
#include <MHK_Button.h>

Button bouton(1, HIGH);

void setup() 
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  Mouse.begin();
  bouton.begin(INPUT_PULLUP);
}

void loop() 
{
  bouton.read(); // update button state
  // put your main code here, to run repeatedly:
  int axe_x = analogRead(A1);
  int axe_y = analogRead(A0);
  int pixel_x = map(axe_x,0,1023,-8,8);
  int pixel_y = map(axe_y,0,1023,8,-8);
  //Serial.print(pixel_x);
  //Serial.print(" ");
  //Serial.println(pixel_y);
  Mouse.move(pixel_x,pixel_y,0);
  if (bouton.pressed())  { Mouse.press();   }
  if (bouton.released()) { Mouse.release(); }
  delay(10);
}
