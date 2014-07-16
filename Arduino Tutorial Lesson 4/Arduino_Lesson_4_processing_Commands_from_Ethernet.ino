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
*/

// Bunch of constants in the form of definitions
// 1 = output debug to serial port, 0 = no debug
#define debug 1 
#define UseEthernet 1 

/* Include needed Lbraries */
#include <SPI.h>
#include <Ethernet.h>
#include <avr/pgmspace.h>
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
#define Out2 5
#define in0 7
#define in1 8

// SPI Pins
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
PROGMEM prog_char webTxt_0[]  = "HTTP/1.1 200 OK\nContent-Type: text/html\nConnection: close\n";
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
enum {hello, goodby, dosomething, web1, web2, web3, web4, web5, 
GET, analogAlljson, analog0, analog1, analog2, analog3, 
digital0, digital1, digital2, digital3, digital4, 
pwm0, pwm1, getmS, digitalAlljson };
//Command Strings
PROGMEM prog_char helloCmd[]     = "Hello";
PROGMEM prog_char goodbyCmd[]    = "Goodby";
PROGMEM prog_char dosomethingCmd[]      = "dosomething";
PROGMEM prog_char web1Cmd[]      = "web1";
PROGMEM prog_char web2Cmd[]      = "web2";
PROGMEM prog_char web3Cmd[]      = "web3";
PROGMEM prog_char web4Cmd[]      = "web4";
PROGMEM prog_char web5Cmd[]      = "web5";
PROGMEM prog_char GETCmd[]   = "GET";
PROGMEM prog_char analogAlljsonCmd[]   = "analogAlljson";
PROGMEM prog_char analog0Cmd[]   = "analog0";
PROGMEM prog_char analog1Cmd[]   = "analog1";
PROGMEM prog_char analog2Cmd[]   = "analog2";
PROGMEM prog_char analog3Cmd[]   = "analog3";
PROGMEM prog_char digital0Cmd[]   = "digital0";
PROGMEM prog_char digital1Cmd[]   = "digital1";
PROGMEM prog_char digital2Cmd[]   = "digital2";
PROGMEM prog_char digital3Cmd[]   = "digital3"; //input
PROGMEM prog_char digital4Cmd[]   = "digital4"; //input
PROGMEM prog_char pwm0Cmd[]       = "pwm0";
PROGMEM prog_char pwm1Cmd[]       = "pwm1";
PROGMEM prog_char getmSCmd[]       = "getmS";
PROGMEM prog_char digitalAlljsonCmd[]       = "digitalAlljson";


//array of pointers to the above command strings
PROGMEM const char* Cmd_table[] =
{helloCmd,goodbyCmd,dosomethingCmd,web1Cmd,web2Cmd,web3Cmd,web4Cmd,web5Cmd,
GETCmd, analogAlljsonCmd,analog0Cmd,analog1Cmd,analog2Cmd,analog3Cmd,
digital0Cmd,digital1Cmd,digital2Cmd,digital3Cmd,digital4Cmd,
pwm0Cmd,pwm1Cmd, getmSCmd, digitalAlljsonCmd };
int cmdCount = sizeof(Cmd_table) / sizeof(Cmd_table[0]);

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

#if UseEthernet
//Ethernet initialization
  Ethernet.begin(mac, ip, dns1, gateway, subnet);
  server.begin();
#endif
  
  // initialise all the digital outputs
  pinMode(Out0, OUTPUT);      
  pinMode(Out1, OUTPUT);      
  pinMode(Out2, OUTPUT);      
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
  if (CheckSerial()) DoCommand(inputBuffer,"\0"); 
  if (CheckEthernet())
  {
//    Serial.print("ethernetbuffer");
//    Serial.println(ethernetbuffer);
    DoCommand(ethernetbuffer,"\0"); 
    client.stop();
  
  }
  // Do other stuff
}

// Enhanced Command Processor using strtok to strip out command from multi parameter string
boolean DoCommand(char * commandBuffer, char * param)
{
  char* Command; // Command Parameter
  char* Parameter; // Additional Parameter
  char* Parameter2; // Additional Parameter for web related stuff
  int analogVal = -1; // additional parameter converted to analog if possible
  // Get the command from the input string
  Command = strtok(commandBuffer,commandDelimeters); // get the command
  Parameter = strtok(NULL, commandDelimeters); // get the parameter if any
  Parameter2 = strtok(NULL, commandDelimeters); // get the parameter if any
  // check to see if we are in the second loop and if a param has been passed in
  if ((&Parameter[0]=='\0') && (&param[0]!= "\0"))
  {
    Parameter = param; // second time through, this may have a value to re-use
  }
  //if there are more than two parameter they will be ignored for now
  // debug code after here
#if debug  
//  Serial.print(Command); Serial.print("|");
//  Serial.print(Parameter); Serial.print("|");
//  Serial.print(Parameter2); Serial.print("|");
//  Serial.print(param); Serial.print("|");
//  Serial.print("Free Ram = "); Serial.println(freeRam(), DEC);
#endif 
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
   case  GET :             DoCommand(Parameter, Parameter2); break; // command is really the second parameter
   case  analogAlljson :   getAllAnalogjson(client); break;
   case  getmS :           getmSeconds(client); break;
   case  analog0 :         getAnalog(A0); break;
   case  analog1 :         getAnalog(A1); break;
   case  analog2 :         getAnalog(A2); break;
   case  analog3 :         getAnalog(A3); break;
   case  digitalAlljson :  getAllDigitaljson(client); break;
   case  digital0 :        getsetDigital(Out0, analogVal);break;
   case  digital1 :        getsetDigital(Out1, analogVal);break;
   case  digital2 :        getsetDigital(Out2, analogVal);break;
   case  digital3 :        getsetDigital(in0, analogVal);break;
   case  digital4 :        getsetDigital(in1, analogVal);break;
   case  pwm0 :            setPWM(PWM0, analogVal, outparameter);break;
   case  pwm1 :            setPWM(PWM1, analogVal, outparameter);break;
   default :             {
                           Serial.print(getStringfromProgMem(Txt_table,errorText));
                           Serial.println(commandBuffer);
                           break;
                         }
  }

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
Serial.write(charBuffer);         
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
void getAllDigitaljson(EthernetClient cl)
{
  code200(cl);
  jsonDigitalResponse(cl);
}

void getAllAnalogjson(EthernetClient cl)
{
  code200(cl);
  jsonAnalogResponse(cl);
}

void getmSeconds(EthernetClient cl)
{
  code200(cl);
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

// send the json response containing analog value
void jsonDigitalResponse(EthernetClient cl)
{    
    cl.print("{");
    cl.print("\"digital0\":\"");
    cl.print(digitalRead(Out0));
    cl.print("\"");
    cl.print(",\"digital1\":\"");
    cl.print(digitalRead(Out1));
    cl.print("\"");
    cl.print(",\"digital2\":\"");
    cl.print(digitalRead(Out2));
    cl.print("\"");
    cl.print(",\"digital3\":\"");
    cl.print(digitalRead(in0));
    cl.print("\"");
    cl.print(",\"digital4\":\"");
    cl.print(digitalRead(in1));
    cl.print("\"");
    cl.print("}\n"); // close it all off
}

// send the json response containing analog value
void jsonAnalogResponse(EthernetClient cl)
{    
    cl.print("{");
    cl.print("\"analog0\":\"");
    cl.print(analogRead(A0));
    cl.print("\"");
    cl.print(",\"analog1\":\"");
    cl.print(analogRead(A1));
    cl.print("\"");
    cl.print(",\"analog2\":\"");
    cl.print(analogRead(A2));
    cl.print("\"");
    cl.print(",\"analog3\":\"");
    cl.print(analogRead(A3));
    cl.print("\"");
    cl.print("}\n"); // close it all off
}
void code200(EthernetClient client) {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("Connection: close");
    client.println();
}
void code404(EthernetClient client)
{
  client.println("HTTP/1.1 404 Not Found");
  client.println("Content-Type: text/html");
  client.println("Connection: close");
  client.println();
  client.println("<html><head><title>404 - Not Found</title></head><body><h1>404 - Not Found</h1></body></html>");
}

