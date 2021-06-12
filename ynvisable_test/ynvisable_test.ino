#include <RDOT_ECD_I2C_1.0.h>

int i2c_address = 43;         //The i2c address of driver board 4.1
int number_of_segments =1;
//#define Display7x2
//#define Display7x1
#define BarDisplay7



RDOT_ECD ECD(i2c_address, number_of_segments); //ECD Object

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("Hello");
  #ifdef Display7x2
   Serial.println("Display7x2");
   number_of_segments = 15;  //Number of segments on display (1-15)
#elif defined(Display7x1)
 Serial.println("Display7x1");
   number_of_segments = 7;  //Number of segments on display (1-7)
#elif defined(BarDisplay7)
   Serial.println("BarDisplay7");
   number_of_segments = 7;  //Number of segments on display (1-7)
#endif


}

void loop() {

#ifdef Display7x2
   drive7x2display();
#elif defined(Display7x1)
   drive7x1display();
#elif defined(BarDisplay7)
   drivebardisplay();
#endif
}
void driveDirectdisplay()
{
bool new_state[7] = {1,0,1,0,1,0,1}; //Every second segment is dark and the others are bright.
ECD.set(new_state);
}
void    drivebardisplay()
{
  for(int i=0; i<8; i++){ //Counting from -99 to 99.
    ECD.setBar(i);      //Set display to number i
    delay(3000);              //5 seconds delay between each update
  }
}
  void drive7x1display()
{
 for(int i=0; i<10; i++){ //Counting from -99 to 99.
    ECD.setNumber1x7(i);      //Set display to number i
    delay(3000);              //5 seconds delay between each update
  }
}
void drive7x2display()
{
 for(int i=-99; i<100; i++){ //Counting from -99 to 99.
    ECD.setNumber2x7(i);      //Set display to number i
    delay(3000);              //5 seconds delay between each update
  }
}
