#include <SoftwareSerial.h>
#include <Servo.h>
#include "VoiceRecognitionV3.h"
#include <LiquidCrystal_I2C.h>

VR myVR(3,2); 

LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27,16,2);
Servo servo_0;
Servo servo_1;
int pos=0;

uint8_t records[7];
uint8_t buf[64];

int R = 4;
int G = 5;
int B = 6;
int servoPin0=7;
int servoPin1=8;
#define onRecord    (0)
#define offRecord   (1) 
#define happyRecord (2)
#define sadRecord (3)
#define helpRecord (4)

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
  myVR.begin(9600);
  
  Serial.begin(9600);
  Serial.println("Elechouse Voice Recognition V3 Module\r\nControl LED sample");
  
  pinMode(R, OUTPUT);
  pinMode(G, OUTPUT);
  pinMode(B, OUTPUT);
  servo_0.attach(servoPin0);
  servo_1.attach(servoPin1);

  lcd.init();          
  lcd.backlight(); 
  lcd.setCursor(0, 0);          
  lcd.print("     Hello"); 
  
  lcd.setCursor(0, 1);          
  lcd.print("      Miles");

  if(myVR.clear() == 0){
    Serial.println("Recognizer cleared.");
  }else{
    Serial.println("VoiceRecognitionModule Not Found");
    Serial.println("Please Check The Connection And Restart The Arduino.");
    while(1);
  }
  
  if(myVR.load((uint8_t)onRecord) >= 0){
    Serial.println("onRecord loaded");
  }
  
  if(myVR.load((uint8_t)offRecord) >= 0){
    Serial.println("offRecord loaded");
  }
   if(myVR.load((uint8_t)happyRecord) >= 0){
    Serial.println("upRecord loaded");
  }
  
  if(myVR.load((uint8_t)sadRecord) >= 0){
    Serial.println("downRecord loaded");
  }
    if(myVR.load((uint8_t)helpRecord) >= 0){
    Serial.println("helpRecord loaded");
  }
}

void loop()
{
  int ret;
  ret = myVR.recognize(buf, 50);
  if(ret>0){
    switch(buf[1]){
      case onRecord:
          digitalWrite(G,HIGH);
          digitalWrite(R,HIGH);
          digitalWrite(B,HIGH);
          Serial.println("R,G,B on");
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("       ;P");
        break;
      case offRecord:
          digitalWrite(R,LOW);
          digitalWrite(G,LOW);
          digitalWrite(B,LOW);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("       :3");
        break;
      case happyRecord:
         servo_0.write(90);
         servo_1.write(90);
         lcd.clear();
         lcd.setCursor(0, 0);
         lcd.print("       :3");
        break;
      case sadRecord:
         servo_0.write(160);
         servo_1.write(20);
         lcd.clear();
         lcd.setCursor(0, 0);
         lcd.print("       3:");
        break;
      case helpRecord:
         digitalWrite(R,HIGH);
         digitalWrite(G,LOW);
         digitalWrite(B,LOW);
         servo_0.write(160);
         servo_1.write(20);
         lcd.clear();
         lcd.setCursor(0, 0);
         lcd.print("      qmq");
        break;  
      default:
        Serial.println("Record function undefined");
        break;
    }
    printVR(buf);
  }
}
