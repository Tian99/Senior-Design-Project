#include <ESP8266WebServer.h>
#include <FS.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include<SoftwareSerial.h> //Included SoftwareSerial Library
SoftwareSerial s(3,1);

//Set Wifi ssid and password
#define SSID_AP             "NodeMCU_WiFi_Car"    // for AP mode
#define PASSWORD_AP         "12345678"            // for AP mode

IPAddress local_ip(192, 168, 4, 1); //IP for AP mode
IPAddress gateway(192, 168, 4, 1); //IP for AP mode
IPAddress subnet(255, 255, 255, 0); //IP for AP mode
ESP8266WebServer server(80);

//Control the serial communication
int count_v;
int count_a;

int led = 7;

//Define the pins
int STBY = 12; //standby
int keep = 0;
//Motor A
int PWMA = 5; //Speed control
int AIN1 = 14; //Direction
int AIN2 = 16; //Direction
//Motor B
int PWMB = 4; //Speed control
int BIN1 = 15; //Direction
int BIN2 = 13; //Direction
int x;
int y;
//This function takes the parameters passed in the URL(the x and y coordinates of the joystick)
//and sets the motor speed based on those parameters. 
void handleJSData(){
  boolean yDir;
  x = server.arg(0).toInt();
  y = server.arg(1).toInt();
  int aSpeed = abs(y);
  int bSpeed = abs(y);
  //set the direction based on y being negative or positive
  if ( y < 0 ){
    yDir = 0; 
  }
  else { 
    yDir = 1;
  }  
  //adjust to speed of each each motor depending on the x-axis
  //it slows down one motor and speeds up the other proportionately 
  //based on the amount of turning
  aSpeed = constrain(aSpeed + x/2, 0, 1023);
  bSpeed = constrain(bSpeed - x/2, 0, 1023);

  //use the speed and direction values to turn the motors
  //if either motor is going in reverse from what is expected,
  //just change the 2 digitalWrite lines for both motors:
  //!ydir would become ydir, and ydir would become !ydir
  //return an HTTP 200
  server.send(200, "text/plain", "");   
}

void setup()
{
  WiFi.mode(WIFI_AP);
  WiFi.softAP(SSID_AP, PASSWORD_AP);
  Serial.print("Connecting to WiFi...");
  Serial.println("");
  Serial.print("Connected! IP: ");
  Serial.println(WiFi.localIP()); //the IP is needed for connection in STA mode
  //set the pins as outputs
  pinMode(STBY, OUTPUT);

  pinMode(PWMA, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);

  pinMode(PWMB, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);  
  // Debug console
  Serial.begin(115200);
  s.begin(38400);
  //initialize SPIFFS to be able to serve up the static HTML files. 
  if (!SPIFFS.begin()){
    Serial.println("SPIFFS Mount failed");
  } 
  else {
    Serial.println("SPIFFS Mount succesfull");
  }
  //set the static pages on SPIFFS for the html and js
  server.serveStatic("/", SPIFFS, "/joystick.html"); 
  server.serveStatic("/virtualjoystick.js", SPIFFS, "/virtualjoystick.js");
  //call handleJSData function when this URL is accessed by the js in the html file
  server.on("/jsData.html", handleJSData);  
  server.begin();
}
StaticJsonBuffer<1000> jsonBuffer;
JsonObject& root = jsonBuffer.createObject();

void loop()
{
  server.handleClient();
  if (isnan(x) || isnan(y)){
    return;
  }
  if(x > -15000 && x < 15000)
  {
    root["x"] = x;
    root["y"] = y;
    root.printTo(s);
    count_v = 0;
    count_a = 0;
//    delay(100);
  }

  //when Voice is activated yet autopilot is not.
  if(x > 15000)
  {
    //Activate voice recognition
    root["x"] = 20000;
    root["y"] = 20000;
//    if(count_v < 3)
    root.printTo(s);
    count_v ++;
    
//    delay(100);
    
  }

  else if(x < -15000)
  {
    //Activate Auto pilot
    root["x"] = -2000;
    root["y"] = -2000;
//    if(count_a < 3)
    root.printTo(s);
    count_a ++;
//    delay(100); 
    
  }
  
  
}
