#include<SoftwareSerial.h>
#define rxPin 11// Broche 11 en tant que RX, à raccorder sur TX du HC-05
#define txPin 10// Broche10 en tant que TX, à raccorder sur RX du HC-05
SoftwareSerial mySerial(rxPin, txPin);

int inByte = 0; // incoming serial byte

void setup() {
  // define pin modes for tx,rx pins:
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  mySerial.begin(9600);

  // computer serial
  Serial.begin(9600);
}

void loop() {
  if (Serial.available() > 0) { // vérifie la présence de données dans la liaison usb
    mySerial.write(Serial.read());
  }
  delay(100);
}
