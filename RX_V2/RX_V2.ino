
#include <SPI.h>
#include "RF24.h"
#include <U8x8lib.h>

byte addresses[][6] = {"1Node","2Node"};

/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 9 and 10 */

RF24 radio(9,10);

U8X8_SSD1306_128X32_UNIVISION_SW_I2C u8x8(/* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);   // Adafruit Feather ESP8266/32u4 Boards + FeatherWing OLED

struct dataStruct{
  unsigned long _micros;
  int sensorID;
  int alarmCode;
}myData;



// SETUP ------>
void setup() {
  u8x8.begin();
  u8x8.setPowerSave(0);
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  u8x8.setCursor(0,0);
  u8x8.print("SensorID: 01 ");

  Serial.begin(9600);
  Serial.print("Sensor ID: 01 ");

  // NRF24 Stuff  
  radio.begin();
  //radio.setPALevel(RF24_PA_LOW);
  radio.setPALevel(RF24_PA_HIGH);  // Radio power level.  Might not work with lower Arduino power
  
  // Open a writing and reading pipe on each radio, with opposite addresses
  radio.openWritingPipe(addresses[0]);
  radio.openReadingPipe(1,addresses[1]);

  // Start the radio listening for data
  radio.startListening();
}
//end of setup


void loop() {
  if( radio.available()){
    while (radio.available()) {                          // While there is data ready
      radio.read( &myData, sizeof(myData) );             // Get the payload  
  }

    // Send back confirmation
    radio.stopListening();                               // First, stop listening so we can talk  
    radio.write( &myData, sizeof(myData) );              // Send the final one back.      
    radio.startListening();                              // Now, resume listening so we catch the next packets.     
   
    Serial.print(F("Sent response "));
    Serial.print(myData._micros);  
    Serial.print(F(" : "));
    Serial.println(myData.sensorID);
  }
} // Loop
