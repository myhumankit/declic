
#include <Mouse.h>


#include<SoftwareSerial.h>
#define rxPin 11// Broche 11 en tant que RX, à raccorder sur TX du HC-05
#define txPin 10// Broche10 en tant que TX, à raccorder sur RX du HC-05

char receivedChar;

SoftwareSerial mySerial(rxPin, txPin);

int inByte = 0; // incoming serial byte

void setup() {
  Mouse.begin();
  // define pin modes for tx,rx pins:
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  mySerial.begin(9600);

  // computer serial
  //Serial.begin(9600);
}

void loop() {

  while (mySerial.available() > 0) {
    receivedChar = mySerial.read();

    switch (receivedChar) {
      case 'b':
        // déplacement souris up
        Mouse.move(0, -10, 0);
        break;

      case 'c':
        // déplacement souris up left
        Mouse.move(-10, -10, 0);
        break;

      case 'd':
        // déplacement souris up right
        Mouse.move(10, -10, 0);
        break;

      case 'e':
        // déplacement souris down
        Mouse.move(0, 10, 0);
        break;

      case 'f':
        // déplacement souris down left
        Mouse.move(-10, 10, 0);
        break;

      case 'g':
        // déplacement souris down right
        Mouse.move(10, 10, 0);
        break;

      case 'i':
        // déplacement souris right
        Mouse.move(10, 0, 0);
        break;

      case 'h':
        // déplacement souris left
        Mouse.move(-10, 0, 0);
        break;

      case 'j':
        // clic left
        Mouse.click();
        break;

      case 'k':
        // clic right
        Mouse.click(MOUSE_RIGHT);
        break;

      case 'l':
        // long clic start
        Mouse.press();
        break;

      case 'm':
        // long clic stop
        Mouse.release();
        break;
    }

    delay(1); // A little delay for the microcontroler stability

  }

}
