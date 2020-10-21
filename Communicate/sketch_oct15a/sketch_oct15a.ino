#include <ArduinoJson.h>
#include <SoftwareSerial.h>

int data; //Initialized variable to store recieved data

void setup() {
  //Serial Begin at 9600 Baud 
  Serial.begin(115200);
  while (!Serial) continue;
}

void loop() {
  StaticJsonBuffer<1000> jsonBuffer;
   JsonObject& root = jsonBuffer.parseObject(Serial);
  if (root == JsonObject::invalid())
  {
    return;
  }
    //Print the data in the serial monitor
  Serial.println("JSON received and parsed");
  root.prettyPrintTo(Serial);
  Serial.println("");
  Serial.print("X");
  int data1=root["x"];
  Serial.println(data1);
  Serial.print("Y");
  int data2=root["y"];
  Serial.println(data2);
  Serial.println("---------------------xxxxx--------------------");
  Serial.println("");
//  delay(10);
}
