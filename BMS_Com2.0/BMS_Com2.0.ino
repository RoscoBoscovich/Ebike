/*
 BMS COMMS
*/

// Software Serial to host BMS
#include <SoftwareSerial.h>
#include <stdio.h>
#include <string.h>

// hex values to send to BMS
byte get_basic_info_message[] = {0xDD, 0xA5, 0x03, 0x00, 0xFF, 0xFD, 0x77};
byte get_cell_voltage_message[] = {0xDD, 0xA5, 0x04, 0x00, 0xFF, 0xFC, 0x77};

// other vars and constants
byte hex_array[128];
byte nib_array[2];
char message_string[128];
bool receiving = 1;
byte hex_index = 0;
byte i = 0 , index = 0, nib1 = 0, nib2 = 0;
int wait_time = 500;
short checksum = 0;
long checksum_time = 1000;
long current_time = 0, start_time;
int transfer_time = 75;
float voltage, current, residual_capacity, nominal_capacity, temperature1, temperature2;
int cycle_times, cells_in_series;

// set up a new serial object
const byte rxPin = 12;
const byte txPin = 13;
SoftwareSerial SerialBMS (rxPin, txPin);


/////////////////////// functions  ////////////////////////

////// split byte into two nibbles 
void split_byte(byte inByte)
{
        nib1 = ((inByte >> 4) & 0x0F);
        nib2 = ((inByte >> 0) & 0x0F);
        nib_array[0] = nib1;
        nib_array[1] = nib2;
}

////// print nibble array to serial
void print_array(byte nibs[], int nibnum)
{
  for (i=0; i < nibnum; i++)
  {
    print_nib(nibs[i]);
  }
}


////// print the nibbles hex values to serial
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


/////// get the data from the BMS 
byte * get_data(byte hex_array[], int ind)
{
checksum = 0;
start_time = millis();
current_time = millis();
while ((current_time - start_time) < transfer_time){
  
  if (SerialBMS.available()) {
        byte inByte = SerialBMS.read();
        split_byte(inByte);
//        print_nib(nib1);
//        print_nib(nib2);
        hex_array[ind] = nib1;
        hex_array[ind+1] = nib2;
        checksum = checksum + inByte;
        ind += 2;
     }
     current_time = millis();
}
return *hex_array, index = ind;
}

/////// display received data to serial port
void display_basic_info(byte hex_array[],int index)
{ 
  index--;
  short int checksum_in = ~(((short)(16*16*16*(int)hex_array[index-5] + 16*16*(int)hex_array[index-4] + 16*(int)hex_array[index-3] + (int)hex_array[index-2]) - 1) | 0xFF);
  checksum = checksum - 0xDD - 0x03 - 0x1B - 0xFB - 0x6F - 0x77 - 0xA5 + (index - 20);;
  
  if ((int)checksum_in == (int)checksum)
  {    
//      Serial.write("\n\rPassed Basic");
  
    
      float voltage = (float)(16*16*16*(int)hex_array[8] + 16*16*(int)hex_array[9] + 16*(int)hex_array[10] + (int)hex_array[11])/100;
      float current = (float)(16*16*16*(int)hex_array[12] + 16*16*(int)hex_array[13] + 16*(int)hex_array[14] + (int)hex_array[15])/1000;
      float residual_capacity = (float)(16*16*16*(int)hex_array[16] + 16*16*(int)hex_array[17] + 16*(int)hex_array[18] + (int)hex_array[19])/100;
      float nominal_capacity = (float)(16*16*16*(int)hex_array[20] + 16*16*(int)hex_array[21] + 16*(int)hex_array[22] + (int)hex_array[23])/100;
      int cycle_times = (float)(16*16*16*(int)hex_array[24] + 16*16*(int)hex_array[25] + 16*(int)hex_array[26] + (int)hex_array[27]);
      int cells_in_series = (16*(int)hex_array[50] + (int)hex_array[51]);
      float temperature1 =  ((float)(16*16*16*(int)hex_array[54] + 16*16*(int)hex_array[55] + 16*(int)hex_array[56] + (int)hex_array[57]) - 2731)/10;
      float temperature2 =  ((float)(16*16*16*(int)hex_array[58] + 16*16*(int)hex_array[59] + 16*(int)hex_array[60] + (int)hex_array[61]) - 2731)/10;
      int state_of_charge = 16*(int)hex_array[46] + (int)hex_array[47];
          
      Serial.write("\n\rVoltage = ");
      Serial.print(voltage);
      Serial.write("V");

      Serial.write("\n\rCurrent = ");
      Serial.print(current);
      Serial.write("A");

      Serial.write("\n\rResidual Capacity = ");
      Serial.print(residual_capacity);
      Serial.write("Ah");

      Serial.write("\n\rNominal Capacity = ");
      Serial.print(nominal_capacity);
      Serial.write("Ah");

      Serial.write("\n\rState of charge = ");
      Serial.print(state_of_charge);
      Serial.write("%");

      Serial.write("\n\rCycle times = ");
      Serial.print(cycle_times);
      Serial.write("");

      Serial.write("\n\rCells in series = ");
      Serial.print(cells_in_series);
      Serial.write("");

      Serial.write("\n\rTemperature 1 = ");
      Serial.print(temperature1);
      Serial.write("C");

      Serial.write("\n\rTemperature 2 = ");
      Serial.print(temperature2);
      Serial.write("C\r\n\r\n"); 

      
//      Serial.write("\n\rIndex = ");
//      Serial.print(index);
//      Serial.write(""); 
//
//      Serial.write("\n\rChecksum expected = ");
//      checksum += (index - 20);
//      *nib_array = short_to_byte_array(checksum);
//      nib_array[3] += 0x0;
//      print_nib(nib_array[0]);
//      print_nib(nib_array[1]);
//      print_nib(nib_array[2]);
//      print_nib(nib_array[3]);
//      
////   print_array(nib_array, 4);
////      Serial.write(""); 
//      
//      Serial.write("\n\rChecksum in = ");
////      Serial.print(checksum_in);
////      Serial.write(", "); 
//      *nib_array = short_to_byte_array(checksum_in);
//      print_array(nib_array, 4);
//      Serial.write("\n\r\n\r"); 
}else{
  Serial.print("failed basic");
}
}

/////// display received data to serial port
void display_voltage_info(byte hex_array[],int index)
{ 
  index--;
  short int checksum_in = ~(((short)(16*16*16*(int)hex_array[index-5] + 16*16*(int)hex_array[index-4] + 16*(int)hex_array[index-3] + (int)hex_array[index-2]) - 1) | 0xFF);
  checksum = checksum - 0xDD - 0x03 - 0x1B - 0xFB - 0x6F - 0x77 - 0xA5 + (index - 8) ;

  if ((int)checksum_in == (int)checksum)
  {    
//      Serial.write("\n\rPassed Voltage");
      
      float voltage1 = (float)(16*16*16*(int)hex_array[8] + 16*16*(int)hex_array[9] + 16*(int)hex_array[10] + (int)hex_array[11])/1000;
      float voltage2 = (float)(16*16*16*(int)hex_array[12] + 16*16*(int)hex_array[13] + 16*(int)hex_array[14] + (int)hex_array[15])/1000;
      float voltage3 = (float)(16*16*16*(int)hex_array[16] + 16*16*(int)hex_array[17] + 16*(int)hex_array[18] + (int)hex_array[19])/1000;
      float voltage4 = (float)(16*16*16*(int)hex_array[20] + 16*16*(int)hex_array[21] + 16*(int)hex_array[22] + (int)hex_array[23])/1000;
      float voltage5 = (float)(16*16*16*(int)hex_array[24] + 16*16*(int)hex_array[25] + 16*(int)hex_array[26] + (int)hex_array[27])/1000;
      float voltage6 = (float)(16*16*16*(int)hex_array[28] + 16*16*(int)hex_array[29] + 16*(int)hex_array[30] + (int)hex_array[31])/1000;
      float voltage7 = (float)(16*16*16*(int)hex_array[32] + 16*16*(int)hex_array[33] + 16*(int)hex_array[34] + (int)hex_array[35])/1000;
      float voltage8 = (float)(16*16*16*(int)hex_array[36] + 16*16*(int)hex_array[37] + 16*(int)hex_array[38] + (int)hex_array[39])/1000;
      float voltage9 = (float)(16*16*16*(int)hex_array[40] + 16*16*(int)hex_array[41] + 16*(int)hex_array[42] + (int)hex_array[43])/1000;
      float voltage10 = (float)(16*16*16*(int)hex_array[44] + 16*16*(int)hex_array[45] + 16*(int)hex_array[46] + (int)hex_array[47])/1000;
      float voltage11 = (float)(16*16*16*(int)hex_array[48] + 16*16*(int)hex_array[49] + 16*(int)hex_array[50] + (int)hex_array[11])/1000;
      float voltage12 = (float)(16*16*16*(int)hex_array[52] + 16*16*(int)hex_array[53] + 16*(int)hex_array[54] + (int)hex_array[55])/1000;

      Serial.write("\n\rVoltage 1  = ");
      Serial.print(voltage1,3);
      Serial.write("V");

      Serial.write("\n\rVoltage 2  = ");
      Serial.print(voltage2,3);
      Serial.write("V");

      Serial.write("\n\rVoltage 3  = ");
      Serial.print(voltage3,3);
      Serial.write("V");

      Serial.write("\n\rVoltage 4  = ");
      Serial.print(voltage4,3);
      Serial.write("V");

      Serial.write("\n\rVoltage 5  = ");
      Serial.print(voltage5,3);
      Serial.write("V");

      Serial.write("\n\rVoltage 6  = ");
      Serial.print(voltage6,3);
      Serial.write("V");

      Serial.write("\n\rVoltage 7  = ");
      Serial.print(voltage7,3);
      Serial.write("V");

      Serial.write("\n\rVoltage 8  = ");
      Serial.print(voltage8,3);
      Serial.write("V");

      Serial.write("\n\rVoltage 9  = ");
      Serial.print(voltage9,3);
      Serial.write("V");

      Serial.write("\n\rVoltage 10 = ");
      Serial.print(voltage10,3);
      Serial.write("V");

      Serial.write("\n\rVoltage 11 = ");
      Serial.print(voltage11,3);
      Serial.write("V");
      
      Serial.write("\n\rVoltage 12 = ");
      Serial.print(voltage12,3);
      Serial.write("V\r\n\r\n");

      
//      Serial.write("\n\rIndex = ");
//      Serial.print(index);
//      Serial.write(""); 
//
//      Serial.write("\n\rChecksum expected = ");
//    
//      *nib_array = short_to_byte_array(checksum);
//      print_nib(nib_array[0]);
//      print_nib(nib_array[1]);
//      print_nib(nib_array[2]);
//      print_nib(nib_array[3]);
//      
//      print_array(nib_array, 4);
//      Serial.write(""); 
//      
//      Serial.write("\n\rChecksum in = ");
//      Serial.print(checksum_in);
//      Serial.write(", "); 
//      *nib_array = short_to_byte_array(checksum_in);
//      print_nib(nib_array[0]);
//      print_nib(nib_array[1]);
//      print_nib(nib_array[2]);
//      print_nib(nib_array[3]);
//      Serial.write("\n\r\n\r"); 
  }else{
  Serial.print("failed voltage");
}
}

////// convert a short to an array of 4 nibs
byte * short_to_byte_array(short checksum_received)
{
   byte nibL1 = ((checksum_received >> 12) & 0x0F);
   byte nibL2 = ((checksum_received >> 8) & 0x0F);
   byte nibL3 = ((checksum_received >> 4) & 0x0F);
   byte nibL4 = ((checksum_received >> 0) & 0x0F);
   nib_array[0] = nibL1;
   nib_array[1] = nibL2;
   nib_array[2] = nibL3;
   nib_array[3] = nibL4;
   return *nib_array;
}

////// calls the functions to get basic info
void get_basic_info()
{
  SerialBMS.write(get_basic_info_message, sizeof(get_basic_info_message));
  SerialBMS.flush();
  *hex_array = get_data(hex_array, index);
   display_basic_info(hex_array, index);
   index = 0;
}

////// calls the functions to get cell voltages
void get_voltage_info()
{
  SerialBMS.write(get_cell_voltage_message, sizeof(get_cell_voltage_message));
  SerialBMS.flush();
  *hex_array = get_data(hex_array, index);
   display_voltage_info(hex_array, index);
   index = 0;
}


//////////////////////// main ///////////////////////
void setup() 
{
 Serial.begin(9600);
 SerialBMS.begin(9600);
}

void loop() {
  get_basic_info();
  get_voltage_info();
}
