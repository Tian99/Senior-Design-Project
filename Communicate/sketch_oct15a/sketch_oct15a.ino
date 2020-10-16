int data; //Initialized variable to store recieved data

void setup() {
  //Serial Begin at 9600 Baud 
  Serial.begin(115200);
}

void loop() {
  data = Serial.read(); //Read the serial data and store it
  delay(10);
  Serial.println(data);
}
