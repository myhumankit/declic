void setup() {
  // put your setup code here, to run once:
  pinMode(1, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  int bouton = digitalRead(1);
  Serial.println(bouton);

  delay(100);
}
