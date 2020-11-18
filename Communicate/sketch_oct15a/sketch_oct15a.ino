#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include "VoiceRecognitionV3.h"

VR myVR(2,3); //2:RX 3:TX

uint8_t record[7]; //save record
uint8_t buf[64];

SoftwareSerial interface (10, 11); // RX, TX

int group = 0;
int led_voice = 13;
int led_server = 12;
int led_forward = 9;
int led_left = 8;
int led_right = 7;
int led_backward = 6;
int led_pilot = 5;
#define switchRecord        (0)

#define group0Record1       (1) 
#define group0Record2       (2) 
#define group0Record3       (3) 
#define group0Record4       (4) 
#define group0Record5       (5) 
#define group0Record6       (6) 

#define group1Record1       (7) 
#define group1Record2       (8) 
#define group1Record3       (9) 
#define group1Record4       (10) 
#define group1Record5       (11) 
#define group1Record6       (12) 

void setup() {
  //Serial Begin at 9600 Baud
  myVR.begin(9600);
  Serial.begin(115200);

  //Error check server
  while (!interface) continue;
  pinMode(led_voice, OUTPUT);
  pinMode(led_server, OUTPUT);
  pinMode(led_pilot, OUTPUT);
  pinMode(led_forward, OUTPUT);
  pinMode(led_backward, OUTPUT);
  pinMode(led_left, OUTPUT);
  pinMode(led_right, OUTPUT);
  
  //Load the records for voice recognition
  record[0] = switchRecord;
  record[1] = group0Record1;
  record[2] = group0Record2;
  record[3] = group0Record3;
  record[4] = group0Record4;
  record[5] = group0Record5;
  record[6] = group0Record6;
  group = 0;
  if(myVR.load(record, 7) >= 0){
    printRecord(record, 7);
    Serial.println(F("loaded."));
  }

   interface.begin(38400); 
}

bool a_voice = false;
bool a_pilot = false;
int distance1 = 0;
int distance2 = 200;
int distance3 = 400;
int distance4 = 600;
int current = 0;
void loop() {
  //Activate voice recognition

  //Activate autopilot
  if(a_pilot)
  {
     digitalWrite(led_voice, LOW);
     digitalWrite(led_pilot, HIGH);
     current += 1;
     if(current > distance1 && current < distance2 )
     {
        digitalWrite(led_forward, HIGH);
        digitalWrite(led_right, LOW);
     }
     else if(current > distance2 && current < distance3 )
     {
        digitalWrite(led_forward, LOW);
        digitalWrite(led_right, HIGH);
     }

     else if(current > distance3 && current < distance4 )
     {
        digitalWrite(led_right, LOW);
        digitalWrite(led_backward, HIGH);
     }

     
     else if(current > distance4 && current < 800)
     {
        digitalWrite(led_backward, LOW);
        digitalWrite(led_left, HIGH);
     }
 
     if(current > 800) 
     {
       digitalWrite(led_left, LOW);
       current = 0;
     }

  }

  
  else if(a_voice)
  {
     digitalWrite(led_voice, HIGH);
     myVR.listen();
        voice();
  }
  
  else
  {
    digitalWrite(led_voice, LOW);
    digitalWrite(led_pilot, LOW);
  }
  digitalWrite(led_server, HIGH);
  interface.listen();
  print_server();
}
void voice()
{

  int ret;
  ret = myVR.recognize(buf, 50);
  if(ret>0){
    switch(buf[1]){
        case group0Record1:
        digitalWrite(led_left, LOW);
        digitalWrite(led_right, LOW);
      break;
        case group0Record2:
        digitalWrite(led_forward, LOW);
        digitalWrite(led_backward, LOW);
      break;
      case group0Record3:
        digitalWrite(led_forward, HIGH);
        digitalWrite(led_backward, LOW);
      break;
      case group0Record4:
        digitalWrite(led_forward, LOW);
        digitalWrite(led_backward, HIGH);
      break;
      case group0Record5:
        digitalWrite(led_right, LOW);
        digitalWrite(led_left, HIGH);
      break;
      case group0Record6:
        digitalWrite(led_left, LOW);
        digitalWrite(led_right, HIGH);
      break;
      case switchRecord:
//      if(digitalRead(led_voice) == HIGH){
//          digitalWrite(led_voice, LOW);
//        }else{
//          digitalWrite(led_voice, HIGH);
//        }
        
        if(group == 0){
          group = 1;
          myVR.clear();
          record[0] = switchRecord;
          record[1] = group1Record1;
          record[2] = group1Record2;
          record[3] = group1Record3;
          record[4] = group1Record4;
          record[5] = group1Record5;
          record[6] = group1Record6;
          if(myVR.load(record, 7) >= 0){
            printRecord(record, 7);
            Serial.println(F("loaded."));
          }
        }else{
          group = 0;
          myVR.clear();
          record[0] = switchRecord;
          record[1] = group0Record1;
          record[2] = group0Record2;
          record[3] = group0Record3;
          record[4] = group0Record4;
          record[5] = group0Record5;
          record[6] = group0Record6;
          if(myVR.load(record, 7) >= 0){
            printRecord(record, 7);
            Serial.println(F("loaded."));
          }
        }
        break;
      default:
        break;
    }
    /** voice recognized */
    printVR(buf);
  }
}


void print_server()
{
  StaticJsonBuffer<50> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(interface);
  
  if (root == JsonObject::invalid())
  {
    return;
  }

  if((int)root["x"] < -150 && (int)root["y"] < -150)
  {
    //Activate auto pilot
    a_pilot = true;
    a_voice = false;
  }
  else if((int)root["x"] > 150 && (int)root["y"] > 150)
  {
    //Activate voice recognition.
    a_voice = true;
    a_pilot = false;
    
  }
  //When both cases are false
  else if((int)root["x"] < 150 &&(int)root["x"] > -150 && (int)root["y"] < 150 &&(int)root["y"] > -150 )
  {
    a_voice = false;
    a_pilot = false;  

    if((int)root["y"] > 0)
    {
      digitalWrite(led_forward, HIGH);
      digitalWrite(led_backward, LOW);
    }
    else if((int)root["y"] < 0)
    {
      digitalWrite(led_forward, LOW);
      digitalWrite(led_backward, HIGH);
    }
    if((int)root["x"] < 0)
    {
      digitalWrite(led_left, HIGH);
      digitalWrite(led_right, LOW);
    }
    else if((int)root["x"] > 0)
    {
      digitalWrite(led_left, LOW);
      digitalWrite(led_right, HIGH);
    }
    else if((int)root["x"] == 0 && (int)root["y"] == 0)
    {
      digitalWrite(led_left, LOW);
      digitalWrite(led_right, LOW);
      digitalWrite(led_forward, LOW);
      digitalWrite(led_backward, LOW);
    }
  }

//  //Print the data in the serial monitor
//  Serial.println("JSON received and parsed");
////  root.prettyPrintTo(Serial);
//  Serial.println("");
//  Serial.print("X");
//  int data1=root["x"];
//  Serial.println(data1);
//  Serial.print("Y");
//  int data2=root["y"];
//  Serial.println(data2);
//  Serial.println("---------------------xxxxx--------------------");
//  Serial.println("");
////  delay(10);

}

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
//  Serial.println("\r\n");
  Serial.println();
}

void printRecord(uint8_t *buf, uint8_t len)
{
  Serial.print(F("Record: "));
  for(int i=0; i<len; i++){
    Serial.print(buf[i], DEC);
    Serial.print(", ");
  }
}
