/*
	RDOT_ECD.h - Library for driving rdot's electrochromic display
	Created by Philip Holgersson - rdot AB, Nov 22, 2019
*/

#ifndef _RDOT_ECD
#define _RDOT_ECD

#include "Arduino.h"
#include "Wire.h"

class RDOT_ECD
{
	public:
		RDOT_ECD(char address, int number_of_segments);
		void set(bool next_state[]);
		void setNumber1x7(int digit);
		void setNumber2x7(int digit);
		void setBar(int digit);
		void refresh();
		void setCounterVoltage(float voltage);
		void setSwitchingTime(int time);
		void sendData(String data);
		void reset();
	private: 
		char _address;
		int _number_of_segments;
};

#endif