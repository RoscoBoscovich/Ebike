/*
 BMS COMMS
*/

// Software Serial to host BMS
#include <SoftwareSerial.h>
#include <stdio.h>
#include <string.h>



// HEX values to send to BMS
byte message[] = {0xDD, 0xA5, 0x05, 0x00, 0xFF, 0xFB, 0x77} ;

byte hex_received[64];
char char_transmit[64];
char message_string[64];

bool receiving = 1;
byte hex_index = 0;
byte index = 0;
int wait_time = 500;

// set up a new serial object
const byte rxPin = 12;
const byte txPin = 13;
SoftwareSerial SerialBMS (rxPin, txPin);

/// Function to convert hex values to thier printable equivilents
void hex_array_to_string(byte array[], unsigned int len, char buffer[])
{
    for (unsigned int i = 0; i < len; i++)
    {
        byte nib1 = (array[i] >> 4) & 0x0F;
        byte nib2 = (array[i] >> 0) & 0x0F;
        buffer[i*2+0] = nib1  < 0xA ? '0' + nib1  : 'A' + nib1  - 0xA;
        buffer[i*2+1] = nib2  < 0xA ? '0' + nib2  : 'A' + nib2  - 0xA;
    }
    buffer[len*2] = '\0';
}


//////////////// SETUP  /////////////////

void setup() {
  
  // initialize both serial ports:
  Serial.begin(9600);
  SerialBMS.begin(9600);
  
  // Send the initialiser to the BMS and the input/output string array to the PC Serial
  Serial.write("\n");
  Serial.write("Transmitted = ");
  hex_array_to_string(message, sizeof(message), message_string);
  Serial.write(message_string, 2*sizeof(message));
  Serial.write("\n");
  Serial.write("Received = ");
  SerialBMS.write(message, sizeof(message));
}



//////////////  MAIN  ////////////////
void loop() {

  while (receiving == 1){

  /// While BMS serial is available and sending, take all values and f
  while (SerialBMS.available()) {
    int inByte = SerialBMS.read();
    hex_received[hex_index] = inByte;
    hex_index++;
    //Serial.write(inByte);
    if (hex_index == 1)
    {
      hex_array_to_string(hex_received, 2, char_transmit);
      Serial.write(char_transmit);
      message_string[index] = char_transmit;
      hex_index = 0;
    }
   
    index++;
  }
}
}
