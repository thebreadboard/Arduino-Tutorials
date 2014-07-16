/*
Title: Lesson 3a. Improved parsing and reduced SRAM consumption - "Case" Statements
 
Description:  Reading a line of text from the Serial Port and sending to a command process function.
the processing function only echos back to the console for this lesson
This sketch also demonstrates the use of compiler directives to turn on and off blocks of functionality

Date created: 5th June 2014
Created By: Peter Oakes

Additional Comments:

Feel free to use and abuse as you wish, I imply NO Warranty

NOTE: defines take no program space untill used so use instead of ints etc when it will never change

Lessons Available
lesson 0. How big is an Empty Sketch anyway
Lesson 1. Reading the Serial input a line at time
Lesson 2. Parsing the line using  "if else if" statements and == or strcmp
Lesson 3a. Preperation for Improved parsing and reduced SRAM consumption - "Case" Statements

*/
/* Include needed Lbraries */

/* End include libraries */

// Bunch of constants in the form of definitions
// 1 = output debug to serial port, 0 = no debug
#define debug 1 
// define the buffer size... 
#define serialbufferSize 50 
#define commandDelimeters "|,.- "

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
  // do other setup here as needed
  
  // Print some pretty instructions
  Serial.println(" Hello, please ask me something");
  Serial.println("type \"Hello\", \"Goodby\", \"web\" or \"dosomething\"" );
  Serial.println("or \"pwmx where x is from 0 to 5\"" );
  Serial.println("or \"analogx where x is from 0 to 5\"" );
  Serial.println("or \"digitalx where x is from 0 to 7\"" );
  Serial.println("or make up your own command");
  Serial.println();
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
    // check the analog value is in the correct range
    if (analogVal < MINPWM || analogVal > MAXPWM) outparameter = false;
  }
  // Standard way to handle commands
  if (strcmp(Command,"Hello")==0){
    Serial.println("Hello back at you ");
    //    Do some other work here
    //    and here
    //    and here
  }  
  else if (strcmp(Command,"Goodby")==0){
    Serial.println("Goodby back at you ");
    //    Do some other work here
    //    and here
    //    and here
  }
  else if (strcmp(Command, "dosomething")==0){
    Serial.println("Like what ?? ");
    //    Do some other work here
    //    and here
    //    and here
  }
  else if (strcmp(Command ,"web1")==0){
    Serial.println("HTTP/1.1 200 OK");
    Serial.println("Content-Type: text/html");
    Serial.println("Connection: close");
    Serial.println();
    Serial.println("<!DOCTYPE html>");
    Serial.println("<html><head><title>Hello from www.thebreadboard.ca</title></head>");
    Serial.println("<body><h1>test 1</h1></body>");
    Serial.println("</html>");
    //    Do some other work here
    //    and here
    //    and here
  }
   else if (strcmp(Command, "web2")==0){
    Serial.println("HTTP/1.1 200 OK");
    Serial.println("Content-Type: text/html");
    Serial.println("Connection: close");
    Serial.println();
    Serial.println("<!DOCTYPE html>");
    Serial.println("<html><head><title>Hello from www.thebreadboard.ca</title></head>");
    Serial.println("<body><h1>test 2</h1></body>");
    Serial.println("</html>");
    //    Do some other work here
    //    and here
    //    and here
  }
  else if (strcmp(Command , "web3")==0){
    Serial.println("HTTP/1.1 200 OK");
    Serial.println("Content-Type: text/html");
    Serial.println("Connection: close");
    Serial.println();
    Serial.println("<!DOCTYPE html>");
    Serial.println("<html><head><title>Hello from www.thebreadboard.ca</title></head>");
    Serial.println("<body><h1>test 3</h1></body>");
    Serial.println("</html>");
    //    Do some other work here
    //    and here
    //    and here
  }
   else if (strcmp(Command ,"web4")==0){
    Serial.println("HTTP/1.1 200 OK");
    Serial.println("Content-Type: text/html");
    Serial.println("Connection: close");
    Serial.println();
    Serial.println("<!DOCTYPE html>");
    Serial.println("<html><head><title>Hello from www.thebreadboard.ca</title></head>");
    Serial.println("<body><h1>test 4</h1></body>");
    Serial.println("</html>");
    //    Do some other work here
    //    and here
    //    and here
  }
  else if (strcmp(Command , "web5")==0){
    Serial.println("HTTP/1.1 200 OK");
    Serial.println("Content-Type: text/html");
    Serial.println("Connection: close");
    Serial.println();
    Serial.println("<!DOCTYPE html>");
    Serial.println("<html><head><title>Hello from www.thebreadboard.ca</title></head>");
    Serial.println("<body><h1>test 5</h1></body>");
    Serial.println("</html>");
    //    Do some other work here
    //    and here
    //    and here
  }
// Now some anlog Read Samples
  else if (strcmp(Command, "analog0")==0){
    Serial.print("Analog Raw = ");
    Serial.print(analogRead(A0));    // read the input pin "A0" is already defined
    Serial.print(" Volts = ");
    Serial.println(double(analogRead(A0))*analogToVolts);    // read the input pin "A0" is already defined
  }
  else if (strcmp(Command, "analog1")==0){
    Serial.print("Analog Raw = ");
    Serial.print(analogRead(A1));    // read the input pin "A0" is already defined
    Serial.print(" Volts = ");
    Serial.println(double(analogRead(A1))*analogToVolts);    // read the input pin "A0" is already defined
  }
  else if (strcmp(Command, "analog2")==0){
    Serial.print("Analog Raw = ");
    Serial.print(analogRead(A2));    // read the input pin "A0" is already defined
    Serial.print(" Volts = ");
    Serial.println(double(analogRead(A2))*analogToVolts);    // read the input pin "A0" is already defined
  }
  else if (strcmp(Command, "analog3")==0){
    Serial.print("Analog Raw = ");
    Serial.print(analogRead(A3));    // read the input pin "A0" is already defined
    Serial.print(" Volts = ");
    Serial.println(double(analogRead(A3))*analogToVolts);    // read the input pin "A0" is already defined
  }
  else if (strcmp(Command, "analog4")==0){
    Serial.print("Analog Raw = ");
    Serial.print(analogRead(A4));    // read the input pin "A0" is already defined
    Serial.print(" Volts = ");
    Serial.println(double(analogRead(A4))*analogToVolts);    // read the input pin "A0" is already defined
  }
  else if (strcmp(Command, "analog5")==0){
    Serial.print("Analog Raw = ");
    Serial.print(analogRead(A5));    // read the input pin "A0" is already defined
    Serial.print(" Volts = ");
    Serial.println(double(analogRead(A5))*analogToVolts);    // read the input pin "A0" is already defined
  }
  //And a few Digitals
  else if (strcmp(Command, "digital0")==0){
    if (outparameter) digitalWrite(D0, !digitalRead(D0));
    Serial.print("Digital 0 = ");
    Serial.println(digitalRead(D0)); 
  }
  else if (strcmp(Command, "digital1")==0){
    if (outparameter) digitalWrite(D1, !digitalRead(D1));
    Serial.print("Digital 1 = ");
    Serial.println(digitalRead(D1)); 
  }
  else if (strcmp(Command, "digital2")==0){
    if (outparameter) digitalWrite(D2, !digitalRead(D2));
    Serial.print("Digital 2 = ");
    Serial.println(digitalRead(D2));
  }
  else if (strcmp(Command, "digital3")==0){
   if (outparameter) digitalWrite(D3, !digitalRead(D3));
   Serial.print("Digital 3 = ");
    Serial.println(digitalRead(D3));
  }
  else if (strcmp(Command, "digital4")==0){
    if (outparameter) digitalWrite(D4, !digitalRead(D4));
    Serial.print("Digital 4 = ");
    Serial.println(digitalRead(D4));
  }
  else if (strcmp(Command, "digital5")==0){
    if (outparameter) digitalWrite(D5, !digitalRead(D5));
    Serial.print("Analog 5 = ");
    Serial.println(digitalRead(D5));
  }
  else if (strcmp(Command, "digital6")==0){
    if (outparameter) digitalWrite(D6, !digitalRead(D6));
    Serial.print("Digital 6 = ");
    Serial.println(digitalRead(D6));
  }
  else if (strcmp(Command, "digital7")==0){
    if (outparameter) digitalWrite(D7, !digitalRead(D7));
    Serial.print("Digital 7 = ");
    Serial.println(digitalRead(D7));
  }
  // now allow the PWM Outputs if we have a valid analog parameter
  else if (strcmp(Command, "pwm0")==0  && outparameter ){
      Serial.print("pwm 0 = ");
      Serial.println(analogVal);    // read the input pin "A1" is already defined
      analogWrite(PWM0,analogVal);    // Set the PWM 0 output
    }
    // now the PWM Outputs
      else if (strcmp(Command, "pwm1")==0  && outparameter ){
      Serial.print("pwm 1 = ");
      Serial.println(analogVal);    // read the input pin "A1" is already defined
      analogWrite(PWM1, analogVal);    // Set the PWM 0 output
    }
    // now the PWM Outputs
      else if (strcmp(Command, "pwm2")==0  && outparameter ){
      Serial.print("pwm 2 = ");
      Serial.println(analogVal);    // read the input pin "A1" is already defined
      analogWrite(PWM2, analogVal);    // Set the PWM 0 output
    }
    // now the PWM Outputs
      else if (strcmp(Command, "pwm3")==0  && outparameter ){
      Serial.print("pwm 3 = ");
      Serial.println(analogVal);    // read the input pin "A1" is already defined
      analogWrite(PWM3, analogVal);    // Set the PWM 0 output
    }
    // now the PWM Outputs
      else if (strcmp(Command, "pwm4")==0  && outparameter ){
      Serial.print("pwm 4 = ");
      Serial.println(analogVal);    // read the input pin "A1" is already defined
      analogWrite(PWM4, analogVal);    // Set the PWM 0 output
    }
    // now the PWM Outputs
      else if (strcmp(Command, "pwm5")==0  && outparameter ){
      Serial.print("pwm 5 = ");
      Serial.println(analogVal);    // read the input pin "A1" is already defined
      analogWrite(PWM5, analogVal);    // Set the PWM 0 output
    }    
  // Catch All
  else {
    Serial.print("I dont understand you \nYou said: ");
    Serial.println(commandBuffer);
    //    Do some other work here
    //    and here
    //    and here
  }
// debug code after here
#if debug  
if (Parameter != '\0'){
  Serial.print((outparameter)? "Remaining Param is numeric and = " : "Remaining Param is none numeric and = " );
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

int isNumeric (const char * s)
{
  while(*s)
  {
    if(!isdigit(*s)) return 0;
    s++;
  }
  return 1;
}

#if debug
// check free ram
int freeRam () 
{
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}
#endif





