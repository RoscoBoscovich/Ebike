#include <SoftwareSerial.h>

/*
  Multiple Serial test

  Receives from the main serial port, sends to the others.
  Receives from serial port 1, sends to the main serial (Serial 0).

  This example works only with boards with more than one serial like Arduino Mega, Due, Zero etc.

  The circuit:
  - any serial device attached to Serial port 1
  - Serial Monitor open on Serial port 0

  created 30 Dec 2008
  modified 20 May 2012
  by Tom Igoe & Jed Roach
  modified 27 Nov 2015
  by Arturo Guadalupi

  This example code is in the public domain.
*/

const int rxpin = 2;
const int txpin = 3; 
SoftwareSerial SerialBMS(txpin, rxpin); 

void setup() {
  // initialize both serial ports:
  Serial.begin(9600);
  SerialBMS.begin(9600);

  Serial.write("0xdd");
  Serial.write("0xa5");
  Serial.write("0x03");
  Serial.write("0x00");
  Serial.write("0xff");
  Serial.write("0xfd");
  Serial.write("0x77");
  SerialBMS.write((byte)0xdd);
  SerialBMS.write((byte)0xa5);
  SerialBMS.write((byte)0x03);
  SerialBMS.write((byte)0x00);
  SerialBMS.write((byte)0xff);
  SerialBMS.write((byte)0xfd);
  SerialBMS.write((byte)0x77);
}

void loop() {
  // read from port 1, send to port


  if (Serial.available() > 0) {
    int v = Serial.read();
    Serial.write((byte) v); // local echo
    SerialBMS.write((byte) v); // send to the other Arduino
  }

  if (SerialBMS.available() > 0) {
    Serial.write(SerialBMS.read()); // Echo what we got
  }
 
}


  

//  // read from port 0, send to port 1:
//  if (Serial.available()) {
//    int inByte = SerialBMS.read();
//    Serial.write(inByte);
  
