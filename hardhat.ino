/**
 ******************************************************************************
 * @file    hardhat.ino
 * @author  JiapengLi (original example)
 * @brief   Demonstration of controlling an RGB LED, servos and an I2C LCD
 *          using the Elechouse Voice Recognition Module V3.
 ******************************************************************************
 * @note
 * This sketch is based on the original example code and modified
 * to fit a custom hardware setup and project logic.
 ******************************************************************************
 * @section HISTORY
 *
 * 2013/06/13  Initial version.
 * 2026/05/02  Modified and extended for the hard hat project.
 ******************************************************************************
 */

#include <SoftwareSerial.h>
#include <Servo.h>
#include "VoiceRecognitionV3.h"
#include <LiquidCrystal_I2C.h>

/**
 * Voice Recognition module instance.
 * RX and TX pins are swapped compared to the original example,
 * matching the physical wiring of this project.
 */
VR myVR(3,2); // RX = pin 3, TX = pin 2

/**
 * I2C LCD configuration (address 0x27, 16 columns, 2 rows)
 */
LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27,16,2); 

/**
 * Servo objects
 */
Servo servo_0;
Servo servo_1;

int pos=0;

/**
 * Buffers used by the voice recognition module
 */
uint8_t records[7]; // Stores loaded voice command records
uint8_t buf[64];    // Buffer for recognition results

/**
 * Pin definitions
 */
int R = 4;
int B = 5;
int G = 6;
int servoPin0=7;
int servoPin1=8;

// Voice command record indexes, you train these with the example from the Voice Recognition Module Library "vr_sample_train"
#define onRecord    (0)
#define offRecord   (1) 
#define upRecord    (2)
#define downRecord  (3)
#define helpRecord  (4)

/**
 * @brief Prints the recognized command signature.
 *        If a character is not printable, its hexadecimal value is printed.
 *
 * @param buf Pointer to the signature buffer
 * @param len Length of the signature
 */
void printSignature(uint8_t *buf, int len)
{
  int i;
  for(i=0; i<len; i++){
    if(buf[i]>0x19 && buf[i]<0x7F){
      Serial.write(buf[i]);
    }
    else{
      Serial.print("[");
      Serial.print(buf[i], HEX);
      Serial.print("]");
    }
  }
}

/**
 * @brief Prints detailed information about the recognized voice command.
 *
 * buf[0]  -> Group mode (FF: none, 0x8n: user group, 0x0n: system group)
 * buf[1]  -> Record number
 * buf[2]  -> Recognizer index
 * buf[3]  -> Signature length
 * buf[4+] -> Signature data
 */
void printVR(uint8_t *buf)
{
  Serial.println("VR Index\tGroup\tRecordNum\tSignature");

  Serial.print(buf[2], DEC);
  Serial.print("\t\t");

  if(buf[0] == 0xFF){
    Serial.print("NONE");
  }
  else if(buf[0]&0x80){
    Serial.print("UG ");
    Serial.print(buf[0]&(~0x80), DEC);
  }
  else{
    Serial.print("SG ");
    Serial.print(buf[0], DEC);
  }
  Serial.print("\t");

  Serial.print(buf[1], DEC);
  Serial.print("\t\t");
  if(buf[3]>0){
    printSignature(buf+4, buf[3]);
  }
  else{
    Serial.print("NONE");
  }
  Serial.println("\r\n");
}

void setup()
{
  /**
   * Initialize serial communication and modules
   */
  myVR.begin(9600);
  
  Serial.begin(115200);
  Serial.println("Elechouse Voice Recognition V3 Module");
  Serial.println("Control LED sample");
  
  pinMode(R, OUTPUT);
  pinMode(G, OUTPUT);
  pinMode(B, OUTPUT);

  servo_0.attach(servoPin0);
  servo_1.attach(servoPin1);

  lcd.init();          // Initialize the LCD
  lcd.backlight();     // Enable LCD backlight
  lcd.clear();

  /**
   * Clear recognizer memory and load voice command records
   */
  if(myVR.clear() == 0){
    Serial.println("Recognizer cleared.");
  }else{
    Serial.println("VoiceRecognitionModule not detected.");
    Serial.println("Please check connection and restart Arduino.");
    while(1);
  }
  
  if(myVR.load((uint8_t)onRecord) >= 0){
    Serial.println("onRecord loaded");
  }
  
  if(myVR.load((uint8_t)offRecord) >= 0){
    Serial.println("offRecord loaded");
  }

  if(myVR.load((uint8_t)upRecord) >= 0){
    Serial.println("upRecord loaded");
  }
  
  if(myVR.load((uint8_t)downRecord) >= 0){
    Serial.println("downRecord loaded");
  }

  if(myVR.load((uint8_t)helpRecord) >= 0){
    Serial.println("helpRecord loaded");
  }
}

/**
 * Main loop handling recognized voice commands
 */
void loop()
{
  int ret;
  ret = myVR.recognize(buf, 50);

  if(ret>0){
    switch(buf[1]){

      case onRecord:
        // Turn RGB LED on
        digitalWrite(G,HIGH);
        digitalWrite(R,HIGH);
        digitalWrite(B,HIGH);

        Serial.println("R,G,B on");

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Lamp: On");
        break;

      case offRecord:
        // Turn RGB LED off
        digitalWrite(R,LOW);
        digitalWrite(G,LOW);
        digitalWrite(B,LOW);

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Lamp: Off");

        Serial.println("R,G,B OFF");
        break;

      case upRecord:
        // Move servos to the "up" position
        servo_0.write(90);
        servo_1.write(90);

        lcd.clear();
        lcd.setCursor(0, 1);
        lcd.print("Ears: Up");

        Serial.println("servo0, servo1 on");
        break;

      case downRecord:
        // Move servos to the "down" position
        servo_0.write(45);
        servo_1.write(135);

        lcd.clear();
        lcd.setCursor(0, 1);
        lcd.print("Ears: Down");

        Serial.println("servo0,servo1 off");
        break;

      case helpRecord:
        // Help / alert mode indication
        digitalWrite(R,HIGH);
        digitalWrite(G,LOW);
        digitalWrite(B,LOW);

        servo_0.write(45);
        servo_1.write(135);

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("HELP!");

        Serial.println("servo0,servo1 off,R Led ON");
        break;

      default:
        Serial.println("Record function undefined");
        break;
    }

    // Print debug information for the recognized command
    printVR(buf);
  }
}
