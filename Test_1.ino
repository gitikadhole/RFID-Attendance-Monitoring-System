#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <MFRC522.h>
#include <SPI.h>
#define SS_PIN D8 //RX slave select
#define RST_PIN D3
#define YellowLed D0
#define RedLed D1
#define GreenLed D2

MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance.

String Link;
String CardID="";

void setup() {
  // put your setup code here, to run once:
  pinMode(YellowLed,OUTPUT);
  pinMode(RedLed,OUTPUT);
  pinMode(GreenLed,OUTPUT);
  // Indicate that Nodemcu is setting up
  digitalWrite(GreenLed, HIGH);
  delay(1000);
  digitalWrite(GreenLed, LOW);
  // 
  Serial.begin(9600);
  
  SPI.begin(); // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522  

  WiFi.begin("Redmi Note 10 Pro","Redmi@S"); // recommended in setup() instead of loop
}

void loop() {
  // put your main code here, to run repeatedly:
  if(WiFi.status() != WL_CONNECTED) // Wait if the wifi connection is 
  {
    digitalWrite(YellowLed, HIGH);
    digitalWrite(RedLed, HIGH);
     
    Serial.println("WiFi disconnected. Waiting for connection...");
    WiFi.begin("Redmi Note 10 Pro","Redmi@S");
    while(WiFi.status() != WL_CONNECTED)
    {
      Serial.print(".") ;
      delay(500);
    }
    digitalWrite(YellowLed, LOW);
    digitalWrite(RedLed, LOW);
    Serial.println();
    Serial.println("Connected !");
    Serial.println(WiFi.localIP());
  }  //Wifi Connection established
  //check if new card is being read and check if card id (UID) is successfully scanned or not
  if(! (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) )
  {
    return;
  }
  //retrieve the UID and store it in string variable
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
      CardID += String(mfrc522.uid.uidByte[i], HEX);
  }
  digitalWrite(YellowLed, HIGH);
  Serial.println("Remove Card and Wait for response.");
  Serial.println(CardID);

  // Send the Card ID to the server 
  // Link = "http://127.0.0.1:8000/mark/?rfid="+CardID;
  Link = "http://192.168.142.53:8000/mark/?rfid="+CardID;
  // http://192.168.142.53:8000/mark/?rfid=rf_10
  HTTPClient http; //object of HttpClient
  WiFiClient client;
  // http.begin(Link);
  http.begin(client, Link);
  int httpCode = http.GET();
  if(httpCode > 0) // successful if positive
  {
    String response = http.getString();
    Serial.println(httpCode);
    Serial.println(response);
    
    //check the response
    if(response == "1")
    {
      digitalWrite(YellowLed, LOW);
      digitalWrite(GreenLed, HIGH);
      Serial.println("Authorized !");
    }
    else{
      digitalWrite(YellowLed, LOW);
      digitalWrite(RedLed, HIGH);
      Serial.println("Denied !");
    }
  }
  else // GET request failed
  {
    digitalWrite(RedLed, HIGH);
    Serial.println("GET request failed");
  }
  // Resetting the variable
  CardID = "";
  Link = "";

  http.end();// Close the connection
  delay(500);
  digitalWrite(YellowLed, LOW);
  digitalWrite(RedLed, LOW);
  digitalWrite(GreenLed, LOW);
}

