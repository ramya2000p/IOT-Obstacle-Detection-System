#include <ESP8266WiFi.h>

const char* ssid = "Ramya's Phone";
const char* password = "ramyaPandian";

WiFiServer server(80);
String state;

const int TRIG_PIN = D5;
const int ECHO_PIN = D6;
const int YELLOW_LED = D3;
const int GREEN_LED = D2;
const int BLUE_LED = D1;

long duration;
int distance;

void setup() {
  Serial.begin(9600);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);

  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(BLUE_LED, LOW);

 WiFi.begin(ssid, password);

  while(WiFi.status() != WL_CONNECTED){
    delay(1000); 
  }

  Serial.print("Controller has connect to WLAN");

  server.begin();
  Serial.println("Server started at port 80");

  Serial.print("URL: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");

}

void loop() {
   //Check if the server has established
  WiFiClient client = server.available();
  if(!client) return;

  //Wait until the client sends the data
  while(!client.available())
  delay(1);

  //Receive message from client
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();

  //set a period for an ultrasonic wave.
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(5);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  duration = pulseIn(ECHO_PIN, HIGH); //Time duration of the feedback wave.

  distance = duration * 0.034 / 2; //Distance in centimeters
 
  Serial.print(distance);

  if(distance > 100 && distance <= 400){ //safe state
    digitalWrite(BLUE_LED, LOW);
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(YELLOW_LED, LOW);

    state = "SAFE";
  }
  else if(distance <= 100 && distance > 20){ //moderate state
    digitalWrite(BLUE_LED, LOW);
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(YELLOW_LED, HIGH);

    state = "MODERATE";
  }
  else if(distance <= 20 && distance >= 2){ //unsafe state
    digitalWrite(BLUE_LED, HIGH);
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(YELLOW_LED, LOW);

    state = "UNSAFE";
  }
  else if(distance < 2 || distance > 400){ //error state
    digitalWrite(BLUE_LED, HIGH);
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(YELLOW_LED, HIGH);
    state = "ERROR";
  }


  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("Refresh: 5");
  client.println("");

  //Personalise your own webpage here with html code:
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.println("<head>");
  client.println("<meta charset='utf-8'/>");
  client.println("<title>Ramya's Obstacle Sensing Module</title>");
  client.println("</head>");
  client.println("<body>");
  client.println("<h1>Obstacle Detector</h1>");

  client.println("<br/><br/>");
  client.println("<h2>The distance is: ");
  client.print(distance);
  client.print("cm</h2>");

  client.println("<h2>The obstacle is in a ");
  client.print(state);
  client.print(" state!<h2>");

  client.println("</body>");
  client.println("</html>");

  delay(1); //1 millisecond delay
 

}
