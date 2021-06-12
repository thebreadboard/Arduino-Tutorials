/*
  Showing number 0-9 on a 7-segment LED display
  Displays the numbers 0-3 on the display
    A
   ---
F |   | B
  | G |
   ---
E |   | C
  |   |
   ---
    D
feel free to change this code as you like
Note: this could be significantly speeded up by using direct port calls
but this is for education and show how it works in an easy way
 */
 
#define DigitDelay 1000 // how long each digit is on for in mS
// You can change the next two lines to adapt for common Anode or Common Cathode bh changing between LOW and HIGH
#define DigitOn HIGH // + side of all LEDS is common so only turn on one at a time
#define DigitOff !DigitOn
#define SegmentOn !DigitOn // - side of LEDs are common across digits and go low to turn on
#define SegmentOff !SegmentOn

// Segment assignment common to all digits, avoid pin 0 and 1 as these are used for programming etc.
int pinA = 2;
int pinB = 3;
int pinC = 4;
int pinD = 5;
int pinE = 6;
int pinF = 7;
int pinG = 8;

// Segment common pin assignments for each digit
int D1 = 9;
int D2 = 10;
int D3 = 11;
int D4 = 12;

// Only run once at startup or reset
void setup() {                
  // initialize the digital pins as outputs.
  pinMode(pinA, OUTPUT);     
  pinMode(pinB, OUTPUT);     
  pinMode(pinC, OUTPUT);     
  pinMode(pinD, OUTPUT);     
  pinMode(pinE, OUTPUT);     
  pinMode(pinF, OUTPUT);     
  pinMode(pinG, OUTPUT);   
  pinMode(D1, OUTPUT);  
  pinMode(D2, OUTPUT);  
  pinMode(D3, OUTPUT);  
  pinMode(D4, OUTPUT);  
}

// runs forever assuming power remains :)
void loop() {
  digitalWrite(D1, DigitOn);
  digitalWrite(D2, DigitOff);
  digitalWrite(D3, DigitOff);
  digitalWrite(D4, DigitOff); 
  //0
  digitalWrite(pinA, SegmentOn);   
  digitalWrite(pinB, SegmentOn);   
  digitalWrite(pinC, SegmentOn);   
  digitalWrite(pinD, SegmentOn);   
  digitalWrite(pinE, SegmentOn);   
  digitalWrite(pinF, SegmentOn);   
  digitalWrite(pinG, SegmentOff);   
  delay(DigitDelay);               // wait
  
  digitalWrite(D1, DigitOff);
  digitalWrite(D2, DigitOn);
  digitalWrite(D3, DigitOff);
  digitalWrite(D4, DigitOff); 
  //1
  digitalWrite(pinA, SegmentOff);   
  digitalWrite(pinB, SegmentOn);   
  digitalWrite(pinC, SegmentOn);   
  digitalWrite(pinD, SegmentOff);   
  digitalWrite(pinE, SegmentOff);   
  digitalWrite(pinF, SegmentOff);   
  digitalWrite(pinG, SegmentOff);   
  delay(DigitDelay);               // wait
  
  digitalWrite(D1, DigitOff);
  digitalWrite(D2, DigitOff);
  digitalWrite(D3, DigitOn);
  digitalWrite(D4, DigitOff); 
  //2
  digitalWrite(pinA, SegmentOn);   
  digitalWrite(pinB, SegmentOn);   
  digitalWrite(pinC, SegmentOff);   
  digitalWrite(pinD, SegmentOn);   
  digitalWrite(pinE, SegmentOn);   
  digitalWrite(pinF, SegmentOff);   
  digitalWrite(pinG, SegmentOn);     
  delay(DigitDelay);               // wait

  digitalWrite(D1, DigitOff);
  digitalWrite(D2, DigitOff);
  digitalWrite(D3, DigitOff);
  digitalWrite(D4, DigitOn); 
  //3
  digitalWrite(pinA, SegmentOn);   
  digitalWrite(pinB, SegmentOn);   
  digitalWrite(pinC, SegmentOn);   
  digitalWrite(pinD, SegmentOn);   
  digitalWrite(pinE, SegmentOff);   
  digitalWrite(pinF, SegmentOff);   
  digitalWrite(pinG, SegmentOn);     
  delay(DigitDelay);               // wait

}
