/*
 BMS COMMS
*/

// Software Serial to host BMS
#include <SoftwareSerial.h>
#include <stdio.h>
#include <string.h>

// hex bytes to send to BMS to have it return info
byte basic_info_message[] = {0xDD, 0xA5, 0x03, 0x00, 0xFF, 0xFD, 0x77};
byte cell_voltage_message[] = {0xDD, 0xA5, 0x04, 0x00, 0xFF, 0xFC, 0x77};
float cell_voltages[12];

byte hex_array[128];  /// hex array + buffer to store returned values from BMS
byte index = 0, current_index = 0;
short checksum = 0;
byte inByte;
long current_time = 0, start_time;
int transfer_time = 100;   // change this is transfer time is too short/long
byte nib1, nib2;
byte nib_array[4];

// set up a new software serial object to communicate with the BMS
const byte rxPin = 12;
const byte txPin = 13;
SoftwareSerial SerialBMS (rxPin, txPin);


/////////////////////// functions  ////////////////////////

// function to print a nibble hex value to serial
void print_nib(byte nib_in)
{
  switch (nib_in) {
          case 0x00:
            Serial.write("0");
            break;
          case 0x01:
           Serial.write("1");
            break;
          case 0x02:
           Serial.write("2");
            break;
          case 0x03:
            Serial.write("3");
            break;
          case 0x04:
          Serial.write("4");
            break;
          case 0x05:
          Serial.write("5");
            break;
          case 0x06:
          Serial.write("6");
            break;
          case 0x07:
          Serial.write("7");
            break;
          case 0x08:
          Serial.write("8");
            break;
          case 0x09:
          Serial.write("9");
            break;
          case 0x0A:
          Serial.write("A");
            break;
          case 0x0B:
          Serial.write("B");
            break;
          case 0x0C:
         Serial.write("C");
            break;
          case 0x0D:
         Serial.write("D");
            break;
          case 0x0E:
          Serial.write("E");
            break;
          case 0x0F:
        Serial.write("F");
            break;
          default:
        Serial.write("#");
            break;
        }
}

//  split byte into two nibbles 
void split_byte(byte inByte)
{
        nib1 = ((inByte >> 4) & 0x0F);
        nib2 = ((inByte >> 0) & 0x0F);
        nib_array[0] = nib1;
        nib_array[1] = nib2;
        return nib1, nib2, nib_array;
}

// get basic info from the BMS
// returns true if checksum passed and info received
bool get_general_info()
{
  SerialBMS.write(basic_info_message, sizeof(basic_info_message));
  *hex_array = get_data(hex_array, index);
  bool got_general = parse_general_info(hex_array, index);
  index = 0;
  return got_general;
}

// calls the functions to get cell voltages
// returns true if checksum passed and voltages received
bool get_cell_voltages()
{
   SerialBMS.write(cell_voltage_message, sizeof(cell_voltage_message));
   *hex_array = get_data(hex_array, index);
   bool got_voltages = parse_cell_voltages(hex_array, index);
   index = 0;
   return got_voltages;
}


// get the data from the BMS, runs this loop for transfer_time(ms)
byte * get_data(byte hex_array[], int current_index)
{
checksum = 0;
start_time = millis();
current_time = millis();
while ((current_time - start_time) < transfer_time)
{
  if (SerialBMS.available()) 
  {
        inByte = SerialBMS.read();
        split_byte(inByte);
//        print_nib(nib1);
//        print_nib(nib2);
        hex_array[current_index] = inByte;
        checksum = checksum + inByte;
        current_index++;
  }
  current_time = millis();
}
return *hex_array, index = current_index;
}


// parse the general info received from the BMS if checksum passed
// returns true if checksum passed
bool parse_general_info(byte hex_array[],int index)
{ 
  short checksum_in = ~(((hex_array[index-3] << 8) |(hex_array[index-2])) | 0xFF);
  checksum = checksum + index - 0x374 ;;

  if ((int)checksum_in == (int)checksum)
  {
    




    
      return true;
  }else
  {
      return false;
  }
}


// parses cell voltage info if checksum passed
// returns true if checksum passed
bool parse_cell_voltages(byte hex_array[],int index)
{ 
  // confirm checksum received is as expected
  short checksum_in = ~(((hex_array[index-3] << 8) |(hex_array[index-2])) | 0xFF);
  checksum = checksum + index - 0x36B ;;
  if (checksum_in == checksum)
  {
   byte cell_index = 0;
   for (byte i=0; i<12; i++)
   {
      short cell_voltage = ((hex_array[4+cell_index] << 8) | (hex_array[5+cell_index]));
      cell_voltages[i] = (float)cell_voltage/1000;
      cell_index +=2;    
   }
      return true;
  }else
  {
      return false;
  }
}

// fucntion to display general BMS stats to serial 
void display_general_info()
{
 //Serial.print("here be general\n\r");
}


// fucntion to display indiviudual cell voltages to serial 
void display_voltages()
{
 for (byte i=0; i< 12; i++)
 {
   Serial.print(cell_voltages[i],3);
   Serial.print("V, ");
 }
 Serial.print("\r\n");
}


///////////////////   main    ////////////////////
void setup() {
 
 Serial.begin(9600);
 SerialBMS.begin(9600);
}

void loop() {
  //bool got_general = get_general_info();
  bool got_voltages = get_cell_voltages();
  if (!got_voltages){}else{display_voltages();}
  //if (!got_general){}else{display_general_info();}
  }
