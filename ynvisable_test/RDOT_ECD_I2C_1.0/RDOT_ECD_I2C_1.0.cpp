#include "Arduino.h"
#include "Wire.h"
#include "RDOT_ECD_I2C_1.0.h"

RDOT_ECD::RDOT_ECD(char address, int number_of_segments)
{
  _address = address; 
  _number_of_segments = number_of_segments;
  Wire.begin();
}

// Update the dipslay to a new state
void RDOT_ECD::set(bool next_state[])
{
  String data = "SETBIN:";
  for(int i = 0; i<_number_of_segments; i++){
    data = data + next_state[i];
  }
  sendData(data);
}

// Function for single seven segment display
void RDOT_ECD::setNumber1x7(int number)
{
  if((number<10) && (number>=0)){
    String data = "SET1X7:";
    data += number;
    sendData(data);
  }
}

// Function for double seven segment display
void RDOT_ECD::setNumber2x7(int number)
{
  if((number<100) && (number>=-99)){
    String data = "SET2X7:";
    data += number;
    sendData(data);
  }
}

// Function for the bar display
void RDOT_ECD::setBar(int number)
{
  if((number < 8) && (number >= 0)){
    String data = "SETBAR:";
    data += number;
    sendData(data);
  }
}

// Set counter voltage
void RDOT_ECD::setCounterVoltage(float voltage)
{
  if((voltage <= 5) && (voltage >= 0)){
    String data = "SETVOL:";
    data += voltage;
    sendData(data);
  }
}

// Set switching time
void RDOT_ECD::setSwitchingTime(int time)
{
  if((time > 0)){
    String data = "SETTIM:";
    data += time;
    sendData(data);
  }
}

// Refresh the dipslay
void RDOT_ECD::refresh()
{
  String data = "REFRES";
  sendData(data);
}

// Reset the display and remove all content
void RDOT_ECD::reset()
{
  String data = "SETRES";
  sendData(data);
}

// Send data to display
void RDOT_ECD::sendData(String data){
  Serial.println(data);
  Wire.beginTransmission(_address);
  Wire.write(data.c_str());
  Wire.endTransmission();
}

