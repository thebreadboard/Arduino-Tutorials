/*
Title: Lesson 3b. Improved parsing and reduced SRAM consumption - "Case" Statements
 
Description:  Reading a line of text from the Serial Port and sending to a command process function.
the processing function performs a real IO function for this lesson
This sketch also demonstrates the use of compiler directives to turn on and off blocks of functionality
Created By: Peter Oakes
Date created: 11th June 2014

Updated 23 June 2016
for IDE 1.6.9 by Peter Oakes
Needed to re-arrange the PROGMEM statements and add some "const char" prefixes to get working again
Additional Comments:
Feel free to use and abuse as you wish, I imply NO Warranty
NOTE: defines take no program space untill used so use instead of ints etc when it will never change
Lessons Available
lesson 0. How big is an Empty Sketch anyway
Lesson 1. Reading the Serial input a line at time
Lesson 2. Parsing the line using  "if else if" statements and == or strcmp
Lesson 3a. Preperation for Improved parsing and reduced SRAM consumption - "Case" Statements
Lesson 3b. Improved parsing and reduced Flash usage, significantly less SRAM consumption and uses "Case" Statements
*/

/* Include needed Lbraries */
#include <avr/pgmspace.h>
/* End include libraries */

// Bunch of constants in the form of definitions
// 1 = output debug to serial port, 0 = no debug
#define debug 1 
// define the buffer size... 
#define serialbufferSize 50 
#define commandDelimeters "|,. " // '-' removed as it is needed as a value leader

#define progMemBuffer 128

// Define some Digital pins
// Dont use the PWM capable ones
#define D0 0
#define D1 1
#define D2 2
#define D3 4
#define D4 7
#define D5 8
#define D6 12
#define D7 13

// define the PWM Pins
#define PWM0 3
#define PWM1 5
#define PWM2 6
#define PWM3 9
#define PWM4 10
#define PWM5 11
#define MAXPWM 255
#define MINPWM 0

#define analogToVolts 5/1023 // Volts per step in ADC 
// End of Constants

// Now the real varibles
char inputBuffer[serialbufferSize]   ; 
int serialIndex = 0; // keep track of where we are in the buffer
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
const char welcomeTxt_0[] PROGMEM = "Hello, please ask me something\ntype \"Hello\", \"Goodby\", \"web\" or \"dosomething\"\n";
const char PROGMEM welcomeTxt_1[]  = "or \"pwmx where x is from 0 to 5\"\nor \"analogx where x is from 0 to 5\"\n";
const char PROGMEM welcomeTxt_2[]  = "or \"digitalx where x is from 0 to 7\"\nor make up your own command\n";
// end of welcome text
const char PROGMEM webTxt_0[]  = "HTTP/1.1 200 OK\nContent-Type: text/html\nConnection: close\n";
const char PROGMEM webTxt_1[]  = "<!DOCTYPE html><html><head><title>Hello from www.thebreadboard.ca</title></head><body>";
const char PROGMEM webTxt_2[]  = "</body></html>\n";
// error text
const char PROGMEM errorTxt_0[]  = "I dont understand you \nYou said: ";
const char PROGMEM helloRepTxt_0[]  = "Hello back at you ";
const char PROGMEM goodbyRepTxt_0[]  = "Goodby back at you ";
const char PROGMEM dosomethingRepTxt_0[]  = "what would you like me to do?";
const char PROGMEM invalidValueRepTxt_0[]  = "Invalid Value, setting unchanged";
#if debug // yes you can even use compiler directives here
const char PROGMEM paramIsNumeric_0[]  = "Remaining Param is numeric and = ";
const char PROGMEM paramIsNotNumeric_0[]  = "Remaining Param is not numeric or invalid, value = ";
#endif

//array of pointers to the above message strings
const char* const Txt_table[] PROGMEM =
{welcomeTxt_0,  welcomeTxt_1,  welcomeTxt_2,  webTxt_0,  webTxt_1,  webTxt_2, errorTxt_0, 
helloRepTxt_0,goodbyRepTxt_0,dosomethingRepTxt_0, invalidValueRepTxt_0, 
#if debug // yes you can even use compiler directives here 
paramIsNumeric_0,paramIsNotNumeric_0
#endif
};

//enumeration to make accessing the command strings easier
enum {hello, goodby, dosomething, web1, web2, web3, web4, web5, analog0, analog1, analog2
, analog3, analog4, analog5, digital0, digital1, digital2, digital3, digital4
, digital5, digital6, digital7, pwm0, pwm1, pwm2, pwm3, pwm4, pwm5};
//Command Strings
const char PROGMEM  helloCmd[]     = "Hello";
const char PROGMEM  goodbyCmd[]    = "Goodby";
const char PROGMEM  dosomethingCmd[]      = "dosomething";
const char PROGMEM  web1Cmd[]      = "web1";
const char PROGMEM  web2Cmd[]      = "web2";
const char PROGMEM  web3Cmd[]      = "web3";
const char PROGMEM  web4Cmd[]      = "web4";
const char PROGMEM  web5Cmd[]      = "web5";
const char PROGMEM  analog0Cmd[]   = "analog0";
const char PROGMEM  analog1Cmd[]   = "analog1";
const char PROGMEM analog2Cmd[]   = "analog2";
const char PROGMEM analog3Cmd[]   = "analog3";
const char PROGMEM analog4Cmd[]   = "analog4";
const char PROGMEM analog5Cmd[]   = "analog5";
const char PROGMEM digital0Cmd[]   = "digital0";
const char PROGMEM digital1Cmd[]   = "digital1";
const char PROGMEM digital2Cmd[]   = "digital2";
const char PROGMEM digital3Cmd[]   = "digital3";
const char PROGMEM digital4Cmd[]   = "digital4";
const char PROGMEM digital5Cmd[]   = "digital5";
const char PROGMEM digital6Cmd[]   = "digital6";
const char PROGMEM digital7Cmd[]   = "digital7";
const char PROGMEM pwm0Cmd[]       = "pwm0";
const char PROGMEM pwm1Cmd[]       = "pwm1";
const char PROGMEM pwm2Cmd[]       = "pwm2";
const char PROGMEM pwm3Cmd[]       = "pwm3";
const char PROGMEM pwm4Cmd[]       = "pwm4";
const char PROGMEM  pwm5Cmd[]       = "pwm5";

//array of pointers to the above command strings
const char* const Cmd_table[] PROGMEM =
{helloCmd,goodbyCmd,dosomethingCmd,web1Cmd,web2Cmd,web3Cmd,web4Cmd,web5Cmd,
analog0Cmd,analog1Cmd,analog2Cmd,analog3Cmd,analog4Cmd,analog5Cmd,
digital0Cmd,digital1Cmd,digital2Cmd,digital3Cmd,digital4Cmd,digital5Cmd,digital6Cmd,digital7Cmd,
pwm0Cmd,pwm1Cmd,pwm2Cmd,pwm3Cmd,pwm4Cmd,pwm5Cmd};
int cmdCount = sizeof(Cmd_table) / sizeof(Cmd_table[0]);

// Function that finds the string in prog mem arrays and gets it into usable space
char buffer[progMemBuffer];
char* getStringfromProgMem(const char* const Table[], int i)
{

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
  // initialise all the digital outputs
  pinMode(D0, OUTPUT);      
  pinMode(D1, OUTPUT);      
  pinMode(D2, OUTPUT);      
  pinMode(D3, OUTPUT);      
  pinMode(D4, OUTPUT);      
  pinMode(D5, OUTPUT);      
  pinMode(D6, OUTPUT);      
  pinMode(D7, OUTPUT);      

  // initialise all the PWM outputs
  pinMode(PWM0, OUTPUT);      
  pinMode(PWM1, OUTPUT);      
  pinMode(PWM2, OUTPUT);      
  pinMode(PWM3, OUTPUT);      
  pinMode(PWM4, OUTPUT);      
  pinMode(PWM5, OUTPUT);      
  
  // initialize serial:
  Serial.begin(9600);
  delay(200);
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
  if (CheckSerial()) DoCommand(inputBuffer); 
  // Do other stuff
}

// Enhanced Command Processor using strtok to strip out command from multi parameter string
boolean DoCommand(char * commandBuffer)
{
  char* Command; // Command Parameter
  char* Parameter; // Additional Parameter
  int analogVal = 0; // additional parameter converted to analog if possible
  
  // Get the command from the input string
  Command = strtok(commandBuffer,commandDelimeters); // get the command
  Parameter = strtok(NULL, commandDelimeters); // get the parameter if any
  //if there are more than one parameter they will be ignored for now

  // Make sure we have an analog value if we are to allow PWM output
  int outparameter = isNumeric (Parameter);
  //if it is a number then convert it
  if (outparameter) 
  {
    analogVal = atoi(Parameter);
  }
  
  // Switch / Case way to handle commands
  int cmdID = findCommand(Command);
  switch( cmdID)
  {
   case  hello :           Serial.println(getStringfromProgMem(Txt_table,helloReply));break;
   case  goodby :          Serial.println(getStringfromProgMem(Txt_table,goodbyReply)); break;
   case  dosomething :     Serial.println(getStringfromProgMem(Txt_table,dosomethingReply)); break;
   case  web1 :            processWebCmd("<H1>Test1</H1>");break;
   case  web2 :            processWebCmd("<H1>Test2</H1>");break;
   case  web3 :            processWebCmd("<H1>Test3</H1>");break;
   case  web4 :            processWebCmd("<H1>Test4</H1>");break;
   case  web5 :            processWebCmd("<H1>Test5</H1>");break;
   case  analog0 :         getAnalog(A0); break;
   case  analog1 :         getAnalog(A1); break;
   case  analog2 :         getAnalog(A2); break;
   case  analog3 :         getAnalog(A3); break;
   case  analog4 :         getAnalog(A4); break;
   case  analog5 :         getAnalog(A5); break;
   case  digital0 :        getsetDigital(D0, analogVal);break;
   case  digital1 :        getsetDigital(D1, analogVal);break;
   case  digital2 :        getsetDigital(D2, analogVal);break;
   case  digital3 :        getsetDigital(D3, analogVal);break;
   case  digital4 :        getsetDigital(D4, analogVal);break;
   case  digital5 :        getsetDigital(D5, analogVal);break;
   case  digital6 :        getsetDigital(D6, analogVal);break;
   case  digital7 :        getsetDigital(D7, analogVal);break;
   case  pwm0 :            setPWM(PWM0, analogVal, outparameter);break;
   case  pwm1 :            setPWM(PWM1, analogVal, outparameter);break;
   case  pwm2 :            setPWM(PWM2, analogVal, outparameter);break;
   case  pwm3 :            setPWM(PWM3, analogVal, outparameter);break;
   case  pwm4 :            setPWM(PWM4, analogVal, outparameter);break;
   case  pwm5 :            setPWM(PWM5, analogVal, outparameter);break;
   default :             {
                           Serial.print(getStringfromProgMem(Txt_table,errorText));
                           Serial.println(commandBuffer);
                           break;
                         }
  }

// debug code after here
#if debug  
if (Parameter != '\0'){
  Serial.print((outparameter)? getStringfromProgMem(Txt_table,isnumericParam) : getStringfromProgMem(Txt_table,notNumericParam) );
  Serial.println(Parameter);  
}
  Serial.print("Free Ram = "); Serial.println(freeRam(), DEC);
#endif  
return true;
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

// check to see if value is nomeric.. only dealing with signed integers
int isNumeric (const char * s)
{
  if (s == NULL || *s == '\0' || isspace(*s)) return 0; // extra protection
  if (*s == '-' || *s == '+') s++; // allow a + or - in the first char space
  while(*s)
  {
    if(!isdigit(*s))
      return 0;
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
// get Digital Channels
void processWebCmd(char* webText)
{
    Serial.print(getStringfromProgMem(Txt_table,webText0));
    Serial.print(getStringfromProgMem(Txt_table,webText1));
    Serial.print(webText);    
    Serial.print(getStringfromProgMem(Txt_table,webText2));
}
