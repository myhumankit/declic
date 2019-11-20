#include <Mouse.h>

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Mouse.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  int axe_x = analogRead(A1);
  int axe_y = analogRead(A0);
  int pixel_x = map(axe_x,0,1023,-10,10);
  int pixel_y = map(axe_y,0,1023,10,-10);
  //Serial.print(pixel_x);
  //Serial.print(" ");
  //Serial.println(pixel_y);
  Mouse.move(pixel_x,pixel_y,0);
  delay(10);
}
