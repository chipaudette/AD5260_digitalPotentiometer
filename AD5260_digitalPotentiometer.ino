/*
  Digital Pot Control
  
  This example controls an Analog Devices AD5260 digital potentiometer.
  The AD5260 has 1 potentiometer channel. Each channel's pins are labeled
  A - connect this to voltage
  W - this is the pot's wiper, which changes when you set it
  B - connect this to ground.
 
 The AD5260 is SPI-compatible,and to command it, you send one byte
 with the resistance value (0 - 255).  
 
 The circuit:
  * A pin of AD5260 connected to +5V
  * B pin of AD5260 connected to ground
  * An LED and a 220-ohm resisor in series connected from each W pin to ground
  * CS - to digital pin 10  (SS pin)
  * SDI - to digital pin 11 (MOSI pin)
  * CLK - to digital pin 13 (SCK pin)

 created 04 Oct 2015
 by Chip Audette

 based on Arduino Example AD5206 by Tom Igoe
  
*/


// include the SPI library:
#include <SPI.h>


// set pin 10 as the slave select for the digital pot:
const int slaveSelectPin = 10;
const int ledPin = 9;
int LED_ON = 1;
void setup() {
  
  //start serial for debugging
  Serial.begin(115200);
  
  // set the slaveSelectPin as an output:
  pinMode (slaveSelectPin, OUTPUT);
  pinMode (slaveSelectPin, HIGH);
  pinMode(ledPin,OUTPUT);
  digitalWrite(ledPin,LED_ON);
  
  // initialize SPI:
  SPI.begin(); 
  SPI.beginTransaction(SPISettings(20000000, MSBFIRST, SPI_MODE0));
}

void loop() {

  // change the resistance from min to max:
  for (int level = 0; level < 255; level += 50) {
    LED_ON = !LED_ON;
    digitalWrite(ledPin,LED_ON);

    //to go as fast as possible, comment out these print statments
//    Serial.print("Commanding value: ");
//    Serial.print(level);
//    Serial.print(", Should Indicate ");
//    Serial.print(((float)level)/255.0*5.0);
//    Serial.println(" V");

    //send value via SPI
    digitalWrite(slaveSelectPin,LOW);// take the SS pin low to select the chip
    SPI.transfer(level);//  send value via SPI:
    digitalWrite(slaveSelectPin,HIGH);// take the SS pin high to de-select the chip

    //wait to allow measurement with voltmeter
    delay(3000);  //comment out this line to go as fast as possible
  }
}


