#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

const int TRIG_PIN = D5;
const int ECHO_PIN = D6;

long duration;
int distanceCM;
int sum;
double average;

#define SSID "Ramya's Phone"
#define PASS "ramyaPandian"

#define AIO_SERVER "io.adafruit.com"
#define AIO_SERVERPORT 1883

#define AIO_USERNAME  "ramya_pandian"
#define AIO_KEY       "aio_kfju38hwK95ghiZJxA06564vy9fW"

WiFiClient client;

Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

Adafruit_MQTT_Publish distance = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/assignment-2-distance-group.safe-state-feed");

//Connect to the server
void MQTT_connect();
void setup() {
  Serial.begin(9600);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  delay(10);

  Serial.println("Adafruit MQTT");
  Serial.println("Connecting to ");
  Serial.println(SSID);
  WiFi.begin(SSID, PASS);
  while(WiFi.status() != 3){
    delay(500);
    Serial.print(".");
  }
  Serial.println();

}

void loop() {
  MQTT_connect();

  sum = 0;
  
  for(int x = 0; x < 10; x++){

  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(5);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  duration = pulseIn(ECHO_PIN, HIGH); //Time duration of the feedback wave.
  distanceCM = duration * 0.034 / 2; //Distance in centimeters
  Serial.println("This is the distance: ");
  Serial.println(distanceCM);

  sum += distanceCM;
  Serial.println("This is the sum: ");
  Serial.println(sum);
  delay(1500);
  }

  average = sum/10;
  Serial.println("THIS IS THE AVERAGE DISTANCE: ");
  Serial.print(average);

  //Write to Adafruit MQTT server
  Serial.print("\nSending average distance value. ");
  Serial.print(average);
  Serial.print("...");
  if(!distance.publish(average))
    Serial.print("Send failed");
  else
    Serial.println("OK!");

  
  if(!mqtt.ping())
    mqtt.disconnect();

}

void MQTT_connect(){
int8_t ret;

 // Stop if already connected.
if(mqtt.connected())
return;

 Serial.print("Connecting to MQTT... ");

 uint8_t retries = 3;
while((ret = mqtt.connect()) != 0){ 
Serial.println(mqtt.connectErrorString(ret));
Serial.println("Retrying MQTT connection in 5 seconds...");
mqtt.disconnect();
delay(10000); 
retries--;
if(retries == 0)
while (1); 
}
Serial.println("MQTT Connected!");
}
