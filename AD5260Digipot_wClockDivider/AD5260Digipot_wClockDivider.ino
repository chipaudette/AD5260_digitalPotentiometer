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

 Created 04 Oct 2015
 by Chip Audette

 based on Arduino Example AD5206 by Tom Igoe

 Updated by Chip Audette in Dec 2015 to add the variable clock speed
  
*/


// include the SPI library:
#include <SPI.h>

//Clockspeed routines from: http://forum.arduino.cc/index.php?topic=223771.0
//define constants for changing the clock speed
#define CLOCK_PRESCALER_1   (0x0)  //  16MHz   (FYI: 15mA   on an Arduino Pro Mini @5V Vcc w. LEDs removed)
#define CLOCK_PRESCALER_2   (0x1)  //   8MHz   (FYI: 10mA   on an Arduino Pro Mini @5V Vcc w. LEDs removed)
#define CLOCK_PRESCALER_4   (0x2)  //   4MHz   (FYI:  8mA   on an Arduino Pro Mini @5V Vcc w. LEDs removed)
#define CLOCK_PRESCALER_8   (0x3)  //   2MHz   (FYI:  6.5mA on an Arduino Pro Mini @5V Vcc w. LEDs removed)
#define CLOCK_PRESCALER_16  (0x4)  //   1MHz   (FYI:  5.5mA on an Arduino Pro Mini @5V Vcc w. LEDs removed)
#define CLOCK_PRESCALER_32  (0x5)  // 500KHz   (FYI:  5mA   on an Arduino Pro Mini @5V Vcc w. LEDs removed)
#define CLOCK_PRESCALER_64  (0x6)  // 250KHz   (FYI:  4.8mA on an Arduino Pro Mini @5V Vcc w. LEDs removed)
#define CLOCK_PRESCALER_128 (0x7)  // 125KHz   (FYI:  4.8mA on an Arduino Pro Mini @5V Vcc w. LEDs removed)
#define CLOCK_PRESCALER_256 (0x8)  // 62.5KHz  (FYI:  4.6mA on an Arduino Pro Mini @5V Vcc w. LEDs removed)

// Choose which default Prescaler option that will be used in this sketch
#define    CLOCK_PRESCALE_DEFAULT   CLOCK_PRESCALER_16


// set pin 10 as the slave select for the digital pot:
const int slaveSelectPin = 10;
const int ledPin = 9;
int LED_ON = 1;
void setup() {
  //change clockspeed
  setPrescale();   // reduce MPU power usage at run-time by slowing down the Main Clock source  
    
  
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

    //wait to allow measurement with voltmeter (scale wait time by the pre-scaler!
    delay(3000/16);  //comment out this line to go as fast as possible
  }
}


/***********************************************************************
****
****   at run-time - MPU speed modifications
****
***********************************************************************/

void setPrescale() {
  
  /* 
   * Setting the Prescale is a timed event, 
   * meaning that two MCU instructions must be executed  
   * within a few clockcycles. 
   */
  
  //To ensure timed events, first turn off interrupts
  cli();                   // Disable interrupts
  CLKPR = _BV(CLKPCE);     //  Enable change. Write the CLKPCE bit to one and all the other to zero. Within 4 clock cycles, set CLKPR again
  CLKPR = CLOCK_PRESCALE_DEFAULT; // Change clock division. Write the CLKPS0..3 bits while writing the CLKPE bit to zero
  sei();                   // Enable interrupts
  
  
  // To get the fastest (and still reliable) ADC (Analog to Digital Converter)
  // operations, when changing the prescale register,
  // you also need to set the ADC_Clk_prescale in the ADCSRA register
  
                                                     // Preferred: 50KHz < ADC_Clk < 200KHz 
#if  CLOCK_PRESCALE_DEFAULT == CLOCK_PRESCALER_1
#define ADC_SPEED 7                                  //ADC_Clk = F_CPU_Pre / 128 => 125KHz
#elif CLOCK_PRESCALE_DEFAULT == CLOCK_PRESCALER_2
#define ADC_SPEED 6                                  //ADC_Clk = F_CPU_Pre /  64 => 125KHz
#elif CLOCK_PRESCALE_DEFAULT == CLOCK_PRESCALER_4
#define ADC_SPEED 5                                  //ADC_Clk = F_CPU_Pre /  32 => 125KHz
#elif CLOCK_PRESCALE_DEFAULT == CLOCK_PRESCALER_8
#define ADC_SPEED 4                                  //ADC_Clk = F_CPU_Pre /  16 => 125KHz
#elif CLOCK_PRESCALE_DEFAULT == CLOCK_PRESCALER_16
#define ADC_SPEED 3                                  //ADC_Clk = F_CPU_Pre /   8 => 125KHz
#elif CLOCK_PRESCALE_DEFAULT == CLOCK_PRESCALER_32
#define ADC_SPEED 2                                  //ADC_Clk = F_CPU_Pre /   4 => 125KHz
#elif CLOCK_PRESCALE_DEFAULT == CLOCK_PRESCALER_64
#define ADC_SPEED 1                                  //ADC_Clk = F_CPU_Pre /   2 => 125KHz
#elif CLOCK_PRESCALE_DEFAULT == CLOCK_PRESCALER_128
#define ADC_SPEED 0                                  //ADC_Clk = F_CPU_Pre /   1 => 125KHz
#elif CLOCK_PRESCALE_DEFAULT == CLOCK_PRESCALER_256
#define ADC_SPEED 0                                  //ADC_Clk = F_CPU_Pre /   1 => 62.5KHz
#endif

  ADCSRA = ( 0x80 | ADC_SPEED);  // Activate ADC and set ADC_Clk 
}


