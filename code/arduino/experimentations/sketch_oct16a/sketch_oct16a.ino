void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  int axe_x = analogRead(A0);
  int axe_y = analogRead(A1);
  Serial.print(axe_x);
  Serial.print(" ");
  Serial.println(axe_y);
  delay(50);
}
