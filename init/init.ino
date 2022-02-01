#include <ESP8266WiFi.h>
#include <SPI.h> 
#include <RFID.h>
#include "FirebaseESP8266.h"  // Install Firebase ESP8266 library

#define FIREBASE_HOST "mqtt-b9578.firebaseio.com" //Without http:// or https:// schemes
#define FIREBASE_AUTH "zeQcXZPtKdHZhKZe5DrX1oTgogiFZ1DW7sB2KWCj"
RFID rfid(D8, D0);       //D8:pin of tag reader SDA. D0:pin of tag reader RST 
unsigned char str[MAX_LEN]; //MAX_LEN is 16: size of the array 

const char ssid[] = "Your Wifi Name";
const char pass[] = "Your Wifi password";

String uidPath= "/";
//Define FirebaseESP8266 data object
FirebaseData firebaseData;

void connect() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\n connected!");
}

void setup()
{

  Serial.begin(115200);
  WiFi.begin(ssid, pass);

  SPI.begin();
  rfid.init();
  
  connect();
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
}
void pushUser (String temp)    //Function to check if an identified tag is registered to allow access
{ 
   Serial.println("PUSHING USER ID: "+temp);

    Firebase.setInt(firebaseData, uidPath+"users/"+temp,0);
}
void loop() {
  if (rfid.findCard(PICC_REQIDL, str) == MI_OK)   //Wait for a tag to be placed near the reader
  { 
    Serial.println("Card found"); 
    String temp = "";                             //Temporary variable to store the read RFID number
    if (rfid.anticoll(str) == MI_OK)              //Anti-collision detection, read tag serial number 
    { 
      Serial.print("The card's ID number is : "); 
      for (int i = 0; i < 4; i++)                 //Record and display the tag serial number 
      { 
        temp = temp + (0x0F & (str[i] >> 4)); 
        temp = temp + (0x0F & str[i]); 
      } 
      Serial.println (temp);
      pushUser (temp);     //Check if the identified tag is an allowed to open tag
    } 
    rfid.selectTag(str); //Lock card to prevent a redundant read, removing the line will make the sketch read cards continually
  }
  rfid.halt();
}
