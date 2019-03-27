/*
 MPL3115A2 Barometric Pressure Sensor Library Example Code
 Uses the MPL3115A2 library to display the current altitude and temperature
 
 Hardware Connections (Breakoutboard to Arduino):
 -VCC = 3.3V
 -SDA = A4 (use inline 10k resistor if your board is 5V)
 -SCL = A5 (use inline 10k resistor if your board is 5V)
 -INT pins can be left unconnected for this demo
 
 During testing, GPS with 9 satellites reported 5393ft, sensor reported 5360ft (delta of 33ft). Very close!
 During testing, GPS with 8 satellites reported 1031ft, sensor reported 1021ft (delta of 10ft).
 
 Available functions:
 .begin() Gets sensor on the I2C bus.
 .readAltitude() Returns float with meters above sealevel. Ex: 1638.94
 .readAltitudeFt() Returns float with feet above sealevel. Ex: 5376.68
 .readPressure() Returns float with barometric pressure in Pa. Ex: 83351.25
 .readTemp() Returns float with current temperature in Celsius. Ex: 23.37
 .readTempF() Returns float with current temperature in Fahrenheit. Ex: 73.96
 .setModeBarometer() Puts the sensor into Pascal measurement mode.
 .setModeAltimeter() Puts the sensor into altimetery mode.
 .setModeStandy() Puts the sensor into Standby mode. Required when changing CTRL1 register.
 .setModeActive() Start taking measurements!
 .setOversampleRate(byte) Sets the # of samples from 1 to 128. See datasheet.
 .enableEventFlags() Sets the fundamental event flags. Required during setup.
 
 */




#include <Wire.h>
#include "SparkFunMPL3115A2.h"
#include <SoftwareSerial.h>


// Initialize vars
MPL3115A2 myPressure; // Create instance of object
double firstAlt;
double altSum = 0;
double altCounter = 0;
double altCalAverage;
int currentAvg = 0;
boolean altimeterSwitch = false;
boolean launch = false;
boolean xbSwitch = false;
char pinWriting = 7;
char highOrLow;
SoftwareSerial XBee(2, 3); // Arduino RX, TX (XBee Dout, Din)



void setup() 
{
  Wire.begin();        // Join i2c bus
  Serial.begin(9600);  // Start serial for output
  myPressure.begin(); // Get sensor online

  // XBee configuration
  XBee.begin(9600); 
  printMenu(); // Print a helpful menu:

  // Sensor configuration
  myPressure.setModeAltimeter(); // Measure altitude above sea level in meters
  myPressure.setOversampleRate(7); // Set Oversample to the recommended 128
  myPressure.enableEventFlags(); // Enable all three pressure and temp event flags

  pinMode(pinWriting, OUTPUT); // Set pin as an OUTPUT
  pinMode(13, OUTPUT);
}



// Determine when to deploy
void loop()
{
  float altitude = myPressure.readAltitudeFt();
  altSum = altSum + altitude;
  altCounter++;
  if (altCounter == 1000) {
    altCalAverage = altSum / altCounter;
    altSum = 0;
    altCounter = 0;
    if (currentAvg == 0){
      firstAlt = altCalAverage;
      Serial.println(firstAlt);
    }
    if (altCalAverage - firstAlt > 500 && launch == false) { // When rocket reaches 500ft above starting point, we know it has launched
      Serial.println(altCalAverage);
      
      launch = true;
    }
    currentAvg++;
  }
  if (altCalAverage - firstAlt < 100 && launch == true) { // If the rocket comes back within 100ft of starting alt and launch is true, it has landed
    altimeterSwitch = true;
    digitalWrite(13, HIGH);
    Serial.println(altimeterSwitch);
  }

   if (XBee.available())
  {
    char c = XBee.read();
    switch (c)
    {
    case 'd':      // If received 'd'
    case 'D':      // or 'D'
      writeDPin(); // Write digital pin
      break;
    case 'r':      // If received 'r'
    case 'R':      // or 'R'
      readDPin();  // Read digital pin
      break;
    }
  }
  
  if (altimeterSwitch && xbSwitch) {
    digitalWrite(pinWriting, HIGH);
    
  } else {
    digitalWrite(pinWriting, LOW);
  }
}















// Write Digital Pin
// Send a 'd' or 'D' to enter.
// Then send a pin #
//   Use numbers for 0-9, and hex (a, b, c, or d) for 10-13
// Then send a value for high or low
//   Use h, H, or 1 for HIGH. Use l, L, or 0 for LOW
void writeDPin()
{
  while (XBee.available() < 2)
    ; // Wait for pin and value to become available
  pinWriting = XBee.read();
  highOrLow = ASCIItoHL(XBee.read());

  // Print a message to let the control know of our intentions:
  XBee.print("Setting pin ");
  XBee.print(pinWriting);
  XBee.print(" to ");
  XBee.println(highOrLow ? "HIGH" : "LOW");

  pinWriting = ASCIItoInt(pinWriting); // Convert ASCCI to a 0-13 value
  xbSwitch = bool(highOrLow);
}



// Read Digital Pin
// Send 'r' or 'R' to enter
// Then send a digital pin # to be read
// The Arduino will print the digital reading of the pin to XBee.
void readDPin()
{
  while (XBee.available() < 1)
    ; // Wait for pin # to be available.
  char pin = XBee.read(); // Read in the pin value

  // Print beggining of message
  XBee.print("Pin ");
  XBee.print(pin);

  pin = ASCIItoInt(pin); // Convert pin to 0-13 value
  pinMode(pin, INPUT); // Set as input
  // Print the rest of the message:
  XBee.print(" = "); 
  XBee.println(digitalRead(pin));
}


// ASCIItoHL
// Helper function to turn an ASCII value into either HIGH or LOW
int ASCIItoHL(char c)
{
  // If received 0, byte value 0, L, or l: return LOW
  // If received 1, byte value 1, H, or h: return HIGH
  if ((c == '0') || (c == 0) || (c == 'L') || (c == 'l'))
    return LOW;
  else if ((c == '1') || (c == 1) || (c == 'H') || (c == 'h'))
    return HIGH;
  else
    return -1;
}



// ASCIItoInt
// Helper function to turn an ASCII hex value into a 0-15 byte val
int ASCIItoInt(char c)
{
  if ((c >= '0') && (c <= '9'))
    return c - 0x30; // Minus 0x30
  else if ((c >= 'A') && (c <= 'F'))
    return c - 0x37; // Minus 0x41 plus 0x0A
  else if ((c >= 'a') && (c <= 'f'))
    return c - 0x57; // Minus 0x61 plus 0x0A
  else
    return -1;
}



// printMenu
// A big ol' string of Serial prints that print a usage menu over
// to the other XBee.
void printMenu()
{
  // Everything is "F()"'d -- which stores the strings in flash.
  // That'll free up SRAM for more importanat stuff.
  XBee.println();
  XBee.println(F("Arduino XBee Remote Control!"));
  XBee.println(F("============================"));
  XBee.println(F("Usage: "));
  XBee.println(F("w#nnn - analog WRITE pin # to nnn"));
  XBee.println(F("  e.g. w6088 - write pin 6 to 88"));
  XBee.println(F("d#v   - digital WRITE pin # to v"));
  XBee.println(F("  e.g. ddh - Write pin 13 High"));
  XBee.println(F("r#    - digital READ digital pin #"));
  XBee.println(F("  e.g. r3 - Digital read pin 3"));
  XBee.println(F("a#    - analog READ analog pin #"));
  XBee.println(F("  e.g. a0 - Read analog pin 0"));
  XBee.println();
  XBee.println(F("- Use hex values for pins 10-13"));
  XBee.println(F("- Upper or lowercase works"));
  XBee.println(F("- Use 0, l, or L to write LOW"));
  XBee.println(F("- Use 1, h, or H to write HIGH"));
  XBee.println(F("============================"));  
  XBee.println();
}

