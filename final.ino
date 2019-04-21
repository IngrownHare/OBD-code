//SPI-OBD INFOTAINMENT SCREEN FOR THE 2012 HONDA ACCORD EX-L V6 6MT



//Include all the necessary libraries
#include <Arduino.h>
#include <U8g2lib.h>
#include <OBD.h>
#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

//Start obd library
COBD obd;

//Constructor call for OLED screen. Defines pin locations and protocol to use in the u8g2 library
U8G2_SSD1305_128X64_ADAFRUIT_1_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ 2, /* data=*/ 3, /* cs=*/ 6, /* dc=*/ 5, /* reset=*/ 4);

//Set pins for button and LEDs
int LED_r = 8;
int LED_gr = 7;
int buttonPin = 12;

//Define initial parameters
int m;
float hp;
int mpg;
int rpm;
float rpm_f;
int throt;
int maf;
int load;
int spd;
float torque;
float spd_dec;
int gear_state = 8;
int digits;


//run arduino setup loop
void setup(void) {
  // Start communication with OLED screen
  u8g2.begin();
  // Define pin 12 as input for clutch button and activate the internal pull-up resistor
  pinMode(buttonPin, INPUT_PULLUP);
  // Define pin 7 as output for green LED
  pinMode(LED_gr, OUTPUT);
  // Define pin 8 as output for green LED
  pinMode(LED_r, OUTPUT);
  // start communication with OBD-II UART adapter
  obd.begin();
  // initiate OBD-II connection until success
  while (!obd.init());  
}

//run functional loop
void loop(void) {

// set variable names for PIDs from car. maf in gram/s, load in %, spd in kmh absolute (no negative for reverse), torque in N*m
  obd.readPID(PID_RPM,rpm);
  obd.readPID(PID_MAF_FLOW,maf);
  obd.readPID(PID_ENGINE_LOAD,load);
  obd.readPID(PID_SPEED,spd);
  obd.readPID(PID_THROTTLE,throt);

//set variable "buttonstate" to value of button ("HIGH" if pressed or "LOW" if not)
  int buttonState = digitalRead(buttonPin);

//save integer rpm value to a float. The readPID function can only output integer variables, so this conversion must be done to ensure that decimals can be printed later
  rpm_f = rpm;

// convert speed from km/h to mph because america
  spd_dec = spd*0.621371;

//calculate velocity ratio (m = RPM/mph)
//here are the velocity ratios for each gear, assuming ~32psi on somewhat worn tires on a 2012 accord v6 6mt:
//gear   |   velocity ratio
//     1 |  180 to 225
//     2 |  110 to 140
//     3 |  75 to 90
//     4 |  53 to 65
//     5 |  40 to 51
//     6 |  31 to 39
//     R |  196 to 235
  m = rpm/spd_dec;

//calculate horsepower using torque and rpm, since theoretically, power is the rate of torque
//  hp = throt * (((-3.32318981*pow(10,-13)*pow(rpm,4) + 3.72700936*pow(10,-9)*pow(rpm,3) - 1.31653661*pow(10,-5)*pow(rpm,2) + 5.43420962*pow(10,-2)*rpm + 9.14334742))/100);
  torque = (load*(4.9347*pow(10,-10)*pow(rpm_f,3) - 1.6129*pow(10,-5)*pow(rpm_f,2) + 0.11777*rpm_f - 1.5511))/100;
  hp = torque*rpm_f/5252;

//define 3 significant figures for horsepower output. In other words, if horsepower is less than 10, then 2 decimals will be used. If hp is between 10 and 100, one decimal. etc...

      if(hp < 10){
      digits = 2;
      }
      else if(hp >= 10 && hp < 100){
      digits = 1;
      }
      else{
      digits = 0;
      }

//calculate mpg
  mpg = spd_dec/(maf*0.08985441);

  u8g2.firstPage();
  do {

    //conditional statements for deriving which gear the car is in and printing the corresponding transmission state
    //the button is "LOW" when the clutch pedal is not being pressed
    //couldn't display reverse because the ratios overlap for 1 and R
//      if( m < 183 && m > 180 && buttonState == LOW){
//      u8g2.setFont(u8g2_font_logisoso58_tr);
//      u8g2.setCursor(83, 63);
//      u8g2.print("R");
//      }
      if( m > 180 && m < 225 && rpm > 900 && buttonState == LOW){
      u8g2.setFont(u8g2_font_logisoso58_tr);
      u8g2.setCursor(83, 63);
      u8g2.print(1);
      gear_state = 1; 
      }
      else if( m > 110 && m < 140 && rpm > 900 && buttonState == LOW){
      u8g2.setFont(u8g2_font_logisoso58_tr);
      u8g2.setCursor(83, 63);
      u8g2.print(2);
      gear_state = 2; 
      }
      else if( m > 75 && m < 90 && rpm > 1100 && buttonState == LOW){
      u8g2.setFont(u8g2_font_logisoso58_tr);
      u8g2.setCursor(83, 63);
      u8g2.print(3);
      gear_state = 3;
      }
      else if( m > 53 && m < 65 && rpm > 1100 && buttonState == LOW){
      u8g2.setFont(u8g2_font_logisoso58_tr);
      u8g2.setCursor(83, 63);
      u8g2.print(4);
      gear_state = 4;
      }
      else if( m > 40 && m < 51 && rpm > 1100 && buttonState == LOW){
      u8g2.setFont(u8g2_font_logisoso58_tr);
      u8g2.setCursor(83, 63);
      u8g2.print(5);
      gear_state = 5;
      }
      else if( m > 31 && m < 39 && rpm > 1100 && buttonState == LOW){
      u8g2.setFont(u8g2_font_logisoso58_tr);
      u8g2.setCursor(83, 63);
      u8g2.print(6);
      gear_state = 6;
      }
      else if (buttonState == HIGH){
      u8g2.setFont(u8g2_font_logisoso58_tr);
      u8g2.setCursor(83, 63);
      u8g2.print("C");       
      }
      else {
      u8g2.setFont(u8g2_font_logisoso58_tr);
      u8g2.setCursor(83, 63); 
      u8g2.print("N"); 
      }
//      else if (buttonState == LOW && spd == 0){
//      u8g2.setFont(u8g2_font_logisoso58_tr);
//      u8g2.setCursor(83, 63);
//      u8g2.print("N");
//      } 
//      else if (buttonState == LOW && m > 6){
//      u8g2.setFont(u8g2_font_logisoso58_tr);
//      u8g2.setCursor(83, 63);
//      u8g2.print("N");
//      } 

//Print the labels for miles per gallon and horsepower
          u8g2.setFont(u8g2_font_mercutio_basic_nbp_tr);
          u8g2.setCursor(2, 30);
          u8g2.print("Hp"); 
          u8g2.setCursor(2, 60);
          u8g2.print("Mpg");


//Print value for horsepower, if the engine is running, with 3 significant figures
          if (rpm > 0){
          u8g2.setFont(u8g2_font_logisoso20_tr);
          u8g2.setCursor(30, 30); 
          u8g2.print(hp, digits);
          }
          if(rpm == 0){
          u8g2.setFont(u8g2_font_logisoso20_tr);
          u8g2.setCursor(30, 30); 
          u8g2.print("--");
          }

//Print value for mpg, if the car is moving
          if (spd == 0){
          u8g2.setFont(u8g2_font_logisoso20_tr);
          u8g2.setCursor(30, 60);
          u8g2.print("--");
          }
          else if (spd > 0){
          u8g2.setFont(u8g2_font_logisoso20_tr);
          u8g2.setCursor(30, 60);
          u8g2.print(mpg);            
          }
         
//turn on LEDs at specified RPM ranges. Doesn't work at 6th gear, since there is no gear to shift up to from there
           if(rpm > 1900 && rpm < 2300 && gear_state<6){
              digitalWrite(LED_gr, HIGH);
          }

           if(rpm > 5750 && rpm < 6250 && gear_state<6){
              digitalWrite(LED_r, HIGH);
          }

    
  } while ( u8g2.nextPage() );

//turn off LEDs and set delay to 100 microseconds. This sets the screen refresh rate to 10 Hz
   digitalWrite(LED_gr, LOW);
   digitalWrite(LED_r, LOW);
   delay(100);
//reset "gear_state" to default value.
   gear_state = 8;

}

