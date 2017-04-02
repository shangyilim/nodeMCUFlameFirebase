#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

const char* ssid = "SSID";
const char* password = "password";

WiFiClientSecure client;

//This pin connects the D1 to NodeMCU GPIO 5
int flamePin = 5;
//This pin connects to D2 
int sendPin = 4;
// the default value of the D1 pin when there is no flame
int noFlameValue;

void setup() {
   //set flame pin to input mode
   pinMode(flamePin, INPUT);
   //set led pin to output mode
   pinMode(sendPin, OUTPUT);
   //get default value of the flame pin to indicate no flame
   noFlameValue = digitalRead(flamePin);
  Serial.begin(115200);

// Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  
}

void loop() {
  
  digitalWrite(sendPin, HIGH);
  int flame = digitalRead(flamePin);
  if (flame != noFlameValue)
  {
    Serial.println("FLAME Detected");
    uploadFirebase();
  }
  else
  {
    Serial.println("No flame");
  }
 delay(300);
 digitalWrite(sendPin, LOW);
 delay(200);

}

void uploadFirebase(){
  
 const char* host="app-name.firebaseio.com";
  String postData = "{\"flame\":\"on\"}";
  
  if (client.connect(host, 443)) {
    
      Serial.println("starting to connect");
      
      client.println("POST /flame.json HTTP/1.1");
      client.println("Host: home-sensor-70625.firebaseio.com");
      client.println("Cache-Control: no-cache");
      client.println("Content-Type: application/json");
      client.print("Content-Length: ");
      client.println(postData.length());
      client.println();
      client.println(postData);
      
      long interval = 2000;
      unsigned long currentMillis = millis(), previousMillis = millis();
      
      while(!client.available()){
        if( (currentMillis - previousMillis) > interval ){
      
          Serial.println("Timeout");
          client.stop();     
          return;
        }
        currentMillis = millis();
      }
      
      while (client.connected())
      {
        digitalWrite(sendPin, HIGH);
        
        if ( client.available() )
        {
          char str=client.read();
         Serial.print(str);
        }      
      }
      digitalWrite(sendPin,LOW);
  }
  else{
    Serial.println("unable to connect");
  }
}
