
/*
* Getting Started example sketch for nRF24L01+ radios
* This is an example of how to send data from one node to another using data structures
* Updated: Dec 2014 by TMRh20
*/
#include <SPI.h>
#include "RF24.h"
//#include <U8g2lib.h>
#include <U8x8lib.h>


byte addresses[][6] = {"1Node","2Node"};

/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 9,10 */
RF24 radio(9,10);


// U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);   // Adafruit Feather ESP8266/32u4 Boards + FeatherWing OLED
U8X8_SSD1306_128X32_UNIVISION_SW_I2C u8x8(/* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);   // Adafruit Feather ESP8266/32u4 Boards + FeatherWing OLED

const int buttonPin = 2;     // the number of the pushbutton pin
int buttonState = 0;         // variable for reading the pushbutton status



int onePing;
struct dataStruct{
  unsigned long _micros;
  int sensorID;
  int alarmCode;
}myData;



void setup() { 

 pinMode(buttonPin, INPUT);

  
  myData.sensorID = 42;
  
//  u8g2.begin();
    u8x8.begin();
  u8x8.setPowerSave(0);
        // transfer internal memory to the display

        u8x8.setFont(u8x8_font_chroma48medium8_r);
        
 // u8x8.drawString(0,0,"Sensor ID:");
 // u8x8.drawString(0,0,myData.sensorID);
  u8x8.setCursor(0,0);
  u8x8.print("SensorID: ");
  u8x8.setCursor(11,0);
  u8x8.print(myData.sensorID);
  

  Serial.begin(9600);
  Serial.print("Sensor ID: ");
  Serial.println(myData.sensorID);
  radio.begin();
   radio.setPALevel(RF24_PA_HIGH);
  
  // Open a writing and reading pipe on each radio, with opposite addresses
    radio.openWritingPipe(addresses[1]);
    radio.openReadingPipe(1,addresses[0]);
 
  // Start the radio listening for data
  radio.startListening();
}
void loop() {

  buttonState = digitalRead(buttonPin);
 if (buttonState == HIGH) {
    sendIt();
  }
/*

if (!onePing) {
  myData.alarmCode = 88;
  sendIt();
  onePing++;
}

*/
//delay(2000);
}

void sendIt() {
  
  radio.stopListening();                                    // First, stop listening so we can talk.
  Serial.println(F("Now sending"));
  myData._micros = micros();
  if (!radio.write( &myData, sizeof(myData) )){
    Serial.println(F("failed"));
    u8x8.clearLine(2);
    u8x8.drawString(2,2,"Fail: Write");
  }
       
  radio.startListening();                                    // Now, continue listening
  
  unsigned long started_waiting_at = micros();               // Set up a timeout period, get the current microseconds
  boolean timeout = false;                                   // Set up a variable to indicate if a response was received or not
    
  while ( ! radio.available() ){                             // While nothing is received
    if (micros() - started_waiting_at > 200000 ){            // If waited longer than 200ms, indicate timeout and exit while loop
      timeout = true;
      break;
    }
  } 
  
  if ( timeout ){                                             // Describe the results
    Serial.println(F("Failed, response timed out."));
    u8x8.setFont(u8x8_font_chroma48medium8_r);
    u8x8.clearLine(2);
    u8x8.drawString(2,2,"Fail: Time Out"); 
  } else {
                                                               // Grab the response, compare, and send to debugging spew
    radio.read( &myData, sizeof(myData) );
    unsigned long time = micros();
    Serial.print(F("Sent "));
    Serial.println(time);
    u8x8.setFont(u8x8_font_chroma48medium8_r);
    u8x8.clearLine(2);
    u8x8.drawString(2,2,"Sent!");
    u8x8.setCursor(4,4);
    u8x8.print(time);
    }
      delay(2000);
}
