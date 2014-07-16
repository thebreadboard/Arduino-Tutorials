/*
Title: Lesson 4. Enhancing the program so far to include ethernet input and response

Description:  Now we have mastered reading and responding to cmmands from the serial port
it is now time to extend our skills to include the interet by adding Ethernet support
for both the Wiznet card and the ENC card

Date created: 19th June 2014
Created By: Peter Oakes

Additional Comments:

Feel free to use and abuse as you wish, I imply NO Warranty

NOTE: defines take no program space untill used so use instead of ints etc when it will never change

Lessons Available
lesson 0. How big is an Empty Sketch anyway
Lesson 1. Reading the Serial input a line at time
Lesson 2. Parsing the line using  "if else if" statements and == or strcmp
Lesson 3a. Preperation for Improved parsing and reduced SRAM consumption - "Case" Statements
Lesson 3b. Improved parsing and reduced Flash usage, significantly less SRAM consumption and uses "Case" Statements
Lesson 4. processing commands from Ethernet, extending what we have learned so far to include ethernet for the Wiznet and ENC26 Shields
Lesson 5. Reading and Writing to an SD card, using the Arduino as a WEB server and Data Logger
*/

// Bunch of constants in the form of definitions
// 1 = output debug to serial port, 0 = no debug
#define debug 1 
#define UseEthernet 1 

/* Include needed Lbraries */
#include <SPI.h>
#include <Ethernet.h>
#include <avr/pgmspace.h>

#include <SD.h>
//#include <SdFat.h>
//#include <SdFatUtil.h>
/* End include libraries */


// define the buffer sizes... 
#define ethernetbufferSize 50
#define serialbufferSize 50 
#define progMemBuffer 128

#define commandDelimeters "|,./&? " // '-' removed as it is needed as a value leader

// Define some Digital pins
// Dont use the PWM capable ones
//#define Out0 0 // also used for serial receive on UNO / MEGA (USB)
//#define Out1 1 // also used for serial transmit on UNO / MEGA (USB)
#define Out0 2
#define Out1 3

#define in0 7
#define in1 8

// SPI Pins
#define ADCCS 5  // ADC Chip Select
#define SDCS 4 // SD Card CS so has 3K to ground
#define SS 10 // Used for Ethernet Shield Select
#define MOSI 11
#define MISO 12
#define SCK 13

//I2C Pins
#define SCL A4
#define SDA A5

// define the PWM Pins
#define PWM0 6
#define PWM1 9

#define MAXPWM 255
#define MINPWM 0

#define analogToVolts 5/1023 // Volts per step in ADC 
// End of Constants

byte digitialIO[] = {Out0, Out1, ADCCS, in0, in1};
byte digitalCount = sizeof(digitialIO) / sizeof(byte);
byte analogIO[] = {A0, A1, A3, A3};
byte analogCount = sizeof(analogIO) / sizeof(byte);
byte adcCount = 8;

#if UseEthernet
// Ethernet required variables
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xaa };
IPAddress ip(192,168,1,247);
IPAddress dns1(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);
EthernetServer server(80); // use standard port 80
EthernetClient client;
char ethernetbuffer[ethernetbufferSize];
#endif

// Now the real varibles
char inputBuffer[serialbufferSize]   ;  // we will use the buffer for serial and Ethernet
int serialIndex = 0; // keep track of where we are in the buffer
int ethernetIndex = 0;
enum CallType {webCall, serialCall, nrfCall, none};
CallType commandFrom = none;

// End of real variables

// Strings to be stored into Program space
//enumeration to make accessing the strings easier
enum {welcomeText0, welcomeText1, welcomeText2, webText0, webText1, webText2, errorText, helloReply, 
	goodbyReply, dosomethingReply, invalidValue
#if debug 
	, isnumericParam, notNumericParam
#endif
};

//welcome text
PROGMEM prog_char welcomeTxt_0[]  = "Hello, please ask me something\ntype \"Hello\", \"Goodby\", \"web\" or \"dosomething\"\n";
PROGMEM prog_char welcomeTxt_1[]  = "or \"pwmx where x is from 0 to 3\"\nor \"analogx where x is from 0 to 3\"\n";
PROGMEM prog_char welcomeTxt_2[]  = "or \"digitalx where x is from 0 to 5\"\nor make up your own command\n";
// end of welcome text
PROGMEM prog_char webTxt_0[]  = "HTTP/1.1 200 OK\nContent-Type: text/html\nConnection: close\n\n";
PROGMEM prog_char webTxt_1[]  = "<!DOCTYPE html><html><head><title>Hello from www.thebreadboard.ca</title></head><body>";
PROGMEM prog_char webTxt_2[]  = "</body></html>\n";
// error text
PROGMEM prog_char errorTxt_0[]  = "I dont understand you \nYou said: ";
PROGMEM prog_char helloRepTxt_0[]  = "Hello back at you ";
PROGMEM prog_char goodbyRepTxt_0[]  = "Goodby back at you ";
PROGMEM prog_char dosomethingRepTxt_0[]  = "what would you like me to do?";
PROGMEM prog_char invalidValueRepTxt_0[]  = "Invalid Value, setting unchanged";
#if debug // yes you can even use compiler directives here
PROGMEM prog_char paramIsNumeric_0[]  = "Remaining Param is numeric and = ";
PROGMEM prog_char paramIsNotNumeric_0[]  = "Remaining Param is not numeric or invalid, value = ";
#endif

//array of pointers to the above message strings
PROGMEM const char* Txt_table[] =
{welcomeTxt_0,  welcomeTxt_1,  welcomeTxt_2,  webTxt_0,  webTxt_1,  webTxt_2, errorTxt_0, 
helloRepTxt_0,goodbyRepTxt_0,dosomethingRepTxt_0, invalidValueRepTxt_0, 
#if debug // yes you can even use compiler directives here 
  paramIsNumeric_0,paramIsNotNumeric_0
#endif
};

//enumeration to make accessing the command strings easier
enum {hello, goodby, dosomething, oscilloscope, gauge, analogAlljson, analog, digital,  adc, pwm0, pwm1, getmS, digitalAlljson };
//Command Strings
PROGMEM prog_char helloCmd[]     = "Hello";
PROGMEM prog_char goodbyCmd[]    = "Goodby";
PROGMEM prog_char dosomethingCmd[]      = "dosomething";
PROGMEM prog_char oscilloscopeCmd[]   = "oscilloscope";
PROGMEM prog_char gaugeCmd[]   = "gauge";
PROGMEM prog_char analogAlljsonCmd[]   = "analogAlljson";
PROGMEM prog_char analogCmd[]   = "analog";
PROGMEM prog_char digitalCmd[]   = "digital";
PROGMEM prog_char adcCmd[]   = "adc"; //input
PROGMEM prog_char pwm0Cmd[]       = "pwm0";
PROGMEM prog_char pwm1Cmd[]       = "pwm1";
PROGMEM prog_char getmSCmd[]       = "getmS";
PROGMEM prog_char digitalAlljsonCmd[]       = "digitalAlljson";

//array of pointers to the above command strings
PROGMEM const char* Cmd_table[] =
{helloCmd,goodbyCmd,dosomethingCmd,oscilloscopeCmd, gaugeCmd, analogAlljsonCmd,analogCmd,digitalCmd,adcCmd,pwm0Cmd,pwm1Cmd, getmSCmd, digitalAlljsonCmd };
int cmdCount = sizeof(Cmd_table) / sizeof(Cmd_table[0]);

// SD Card library variables
Sd2Card card;
File webFile;

// Function that finds the string in prog mem arrays and gets it into usable space
char* getStringfromProgMem(const char* Table[], int i)
{
	char buffer[progMemBuffer];
	strcpy_P(buffer, (char*)pgm_read_word(&(Table[i])));
	return buffer;
};

// Search through the comands untill we find one or run out
int findCommand(char* searchText)
{
	int startCount = 0;
	int foundIndex = -1; // -1 = not found
	while (startCount < cmdCount)
	{
		if(strcmp(searchText,getStringfromProgMem(Cmd_table,startCount))==0)
		{
			foundIndex = startCount;
			break;
		}
		startCount++;
	}
	return foundIndex;
}

void setup() 
{ 
	if (!card.init(SPI_HALF_SPEED, 4)) Serial.println("card.init failed!");
	if (!SD.begin(SDCS)) {
		Serial.println("Card failed, or not present");
		// don't do anything more:
		return;
	}
#if UseEthernet
	//Ethernet initialization
	Ethernet.begin(mac, ip, dns1, gateway, subnet);
	server.begin();
#endif

	// initialise all the digital outputs
	pinMode(Out0, OUTPUT);      
	pinMode(Out1, OUTPUT);      
	pinMode(ADCCS, OUTPUT);      
	pinMode(in0, INPUT);    // HIGH turns on the pull up resistor  
	pinMode(in1, INPUT);    // HIGH turns on the pull up resistor      

	// initialise all the PWM outputs
	pinMode(PWM0, OUTPUT);      
	pinMode(PWM1, OUTPUT);      

	// initialize serial:
	Serial.begin(115200);
	// do other setup here as needed

	// Print some pretty instructions
	Serial.print(getStringfromProgMem(Txt_table,welcomeText0));
	Serial.print(getStringfromProgMem(Txt_table,welcomeText1));
	Serial.print(getStringfromProgMem(Txt_table,welcomeText2));
}

void loop() 
{
	// Notice how the main loop is very simple and the functions 
	// seperate the logic into easily manageable parts
	if (CheckSerial()) 
	{
		commandFrom = serialCall;
		DoCommand(inputBuffer); 
	}
	else if (CheckEthernet())
	{
		commandFrom = webCall;
		DoCommand(ethernetbuffer); 
		client.stop();
	}
	else
	{
		commandFrom = none;
	}
	// Do other stuff
}

// Enhanced Command Processor using strtok to strip out command from multi parameter string
boolean DoCommand(char * commandBuffer)
{
	char* Command;		// Command Parameter
	char* Parameter1;	// Additional Parameter
	char* Parameter2;	// Additional Parameter for web related stuff
	char* Parameter3;	// Additional Parameter for web related stuff
	int paramVal1 = -1; // additional parameter converted to analog if possible
	int paramVal2 = -1; // additional parameter converted to analog if possible
	// Get the command from the input string
	Command = strtok(commandBuffer,commandDelimeters);	// get the command
	Parameter1 = strtok(NULL, commandDelimeters);		// get the parameter if any
	Parameter2 = strtok(NULL, commandDelimeters);		// get the parameter if any
	Parameter3 = strtok(NULL, commandDelimeters);		// get the parameter if any
	// check to see if we are processing a WEB get command, if so then move the parameters
	if(strcmp(Command, "GET")==0 && commandFrom == webCall)
	{
		Command = Parameter1;
		Parameter1 = Parameter2;
		Parameter2 = Parameter3;
	}
	// debug code after here
#if debug  
//	  Serial.print(Command); Serial.print("|");
//	  Serial.print(Parameter1); Serial.print("|");
//	  Serial.print(Parameter2); Serial.print("|");
//	  Serial.print(Parameter3); Serial.print("|");
//	  Serial.print(param); Serial.print("|");
	  Serial.print("Free Ram = "); Serial.println(freeRam(), DEC);
#endif 
	// Make sure we have an analog value if we are to allow PWM output
	//if it is a number then convert it
	int outparameter1 = isNumeric (Parameter1);
	if (outparameter1) paramVal1 = atoi(Parameter1);
	int outparameter2 = isNumeric (Parameter2);
	if (outparameter2) paramVal1 = atoi(Parameter2);

	// Switch / Case way to handle commands
	int cmdID = findCommand(Command);
	switch( cmdID)
	{
	case  hello :           Serial.println(getStringfromProgMem(Txt_table,helloReply));break;
	case  goodby :          Serial.println(getStringfromProgMem(Txt_table,goodbyReply)); break;
	case  dosomething :     Serial.println(getStringfromProgMem(Txt_table,dosomethingReply)); break;
	case  analogAlljson :   jsonAnalogResponse(client); break;
	case  getmS :           getmSeconds(client); break;
	case  analog :           getAnalog(paramVal1); break;
	case  digitalAlljson :  jsonDigitalResponse(client); break;
	case  digital :          getsetDigital(paramVal1, paramVal2);break;
	case  adc :             jsonADCResponse(client);break;
	case  pwm0 :            setPWM(PWM0, paramVal1, outparameter1);break;
	case  pwm1 :            setPWM(PWM1, paramVal1, outparameter2);break;
	case  gauge :           sendFileFromSD(client, "lesson4.htm");break;
	case  oscilloscope :    sendFileFromSD(client, "scope.htm");break;
	default :             
          {
		Serial.print(getStringfromProgMem(Txt_table,errorText));
		Serial.println(Command);
		break;
	  }
	}
	return true;
}

void sendFileFromSD(EthernetClient client, char * filename)
{
  if (SD.exists(filename)) {
    client.print(getStringfromProgMem(Txt_table, webText0));
    char c ;
    Serial.println("Reading File");
    webFile = SD.open(filename);
    if (webFile) {
	while(webFile.available()) {
	  c = webFile.read();
	  client.write(c);
	}
	webFile.close();
    }
  } 
}

/*
Checks the serial input for a string, returns true once a '\n' is seen
users can always look at the global variable "serialIndex" to see if characters have been received already
*/
boolean CheckSerial()
{
	boolean lineFound = false;
	// if there's any serial available, read it:
	while (Serial.available() > 0) {
		//Read a character as it comes in:
		//currently this will throw away anything after the buffer is full or the \n is detected
		char charBuffer = Serial.read(); 
		if (charBuffer == '\n') {
			inputBuffer[serialIndex] = 0; // terminate the string
			lineFound = (serialIndex > 0); // only good if we sent more than an empty line
			serialIndex=0; // reset for next line of data
		}
		else if(charBuffer == '\r') {
			// Just ignore the Carrage return, were only interested in new line
		}
		else if(serialIndex < serialbufferSize && lineFound == false) {
			/*Place the character in the string buffer:*/
			inputBuffer[serialIndex++] = charBuffer; // auto increment index
		}
	}// End of While
	return lineFound;
}// End of CheckSerial()

// Basically this does the same as the Serial routine above, 
//just in this case the data is comming from the ethernet port
boolean CheckEthernet()
{
  boolean lineFound = false;
  client = server.available();
  if (client) {
    while (client.connected()) {
      if (client.available()) { 
        char charBuffer = client.read();
        if (charBuffer == '\n' ) {
          ethernetbuffer[ethernetIndex] = 0; // terminate the string
          lineFound = (ethernetIndex > 0); // only good if we sent more than an empty line
          ethernetIndex=0; // reset for next line of data
        }
        else if (charBuffer == '\r'){}//do nothing
        else if(ethernetIndex < ethernetbufferSize && lineFound == false) {
        /*Place the character in the string buffer:*/
        ethernetbuffer[ethernetIndex++] = charBuffer; // auto increment index
        }
      }// end of if client.available()
    return lineFound;
    }// End of CheckEthernet()
  }
}

void getmSeconds(EthernetClient cl)
{
  cl.print(getStringfromProgMem(Txt_table, webText0));
  cl.print("{\"value\":\"");
  cl.print(millis());
  cl.print("\"}\n");
}


// check to see if value is nomeric.. only dealing with signed integers
int isNumeric (const char * s)
{
  if (s == NULL || *s == '\0' || isspace(*s)) return 0; // extra protection
  if (*s == '-' || *s == '+') s++; // allow a + or - in the first char space
  while(*s)
    {
      if(!isdigit(*s)) return 0;
      s++;
    }
  return 1;
}
// end of check is numeric

#if debug
// check free ram
int freeRam () 
{
	extern int __heap_start, *__brkval; 
	int v; 
	return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}
#endif

// output to PWM Channels
void setPWM(int pin, int value, int IsValid)
{
	// check the analog value is in the correct range if not then make isvalid false
	if (((value < MINPWM )|| (value > MAXPWM)) && IsValid) IsValid = false;

	Serial.print("pwm "); Serial.print(pin); Serial.print(" = ");
	Serial.println(value);    // read the input pin "A1" is already defined
	// dont change value if invalid
	if(IsValid) analogWrite(pin, value); else Serial.println(getStringfromProgMem(Txt_table,invalidValue));
}

// get analog Channels
void getAnalog(int pin)
{
	int readValue = analogRead(pin);
	Serial.print("Analog "); Serial.print(pin); Serial.print(" = ");
	Serial.print(readValue);    // read the input pin "Ax" is already defined
	Serial.print(" Volts = ");
	Serial.println(double(readValue)*analogToVolts);    // read the input pin "Ax"
}
// get Digital Channels
void getsetDigital(int pin, int value)
{
	Serial.print("Digital "); Serial.print(pin); Serial.print(" was = "); Serial.println(digitalRead(pin));
	if(value == -1)digitalWrite(pin, !digitalRead(pin)); // invert value
	else if(value == 0)digitalWrite(pin, LOW); // set value low
	else if(value == 1)digitalWrite(pin, HIGH); // set value high
	else Serial.println(getStringfromProgMem(Txt_table,invalidValue));
}

// send the json response containing analog value
void jsonDigitalResponse(EthernetClient cl)
{    
  int first = 0;
    cl.print(getStringfromProgMem(Txt_table, webText0));
    cl.print("{");
    for (int x=0 ; x<digitalCount ;x++)
    {
      if (first != 0) cl.print(","); else first++;
      cl.print("\"digital"); cl.print(x); cl.print("\":\"");
	  cl.print(analogRead(digitialIO[x]));
	  cl.print("\"");
    }
        cl.print("}\n"); // close it all off
}

// send the json response containing analog value
void jsonAnalogResponse(EthernetClient cl)
{    
    int first = 0;
    cl.print(getStringfromProgMem(Txt_table, webText0));
    cl.print("{");
    for (int x=0 ; x<analogCount ;x++)
    {
      if (first != 0) cl.print(",");else first++;
      cl.print("\"analog"); cl.print(x); cl.print("\":\"");
	  cl.print(analogRead(analogIO[x]));
	  cl.print("\"");
    }
        cl.print("}\n"); // close it all off
}
// read the MCP3304 in quad differential mode, non-bit-banging version
void jsonADCResponse(EthernetClient cl)
{
    int first = 0;
    cl.print(getStringfromProgMem(Txt_table, webText0));
    cl.print("{");
    for (int x=0 ; x<adcCount ;x++)
    {
      if (first != 0) cl.print(",");
    else first++;
      cl.print("\"analog"); cl.print(x); cl.print("\":\"");
	  cl.print(readADC(x));
	  cl.print("\"");
    }
    cl.print("}\n"); // close it all off
}

// read a single ADC Channel
uint16_t readADC(int channel)
{  
  uint8_t addr = 0b01100000 | ((channel & 0b111) << 2);
  digitalWrite(ADCCS, LOW);
  SPI.transfer(addr);
  uint8_t b1 = SPI.transfer(0);
  uint8_t b2 = SPI.transfer(0);
  digitalWrite(ADCCS, HIGH);
  return (b1 << 4) | (b2 >> 4);
}

