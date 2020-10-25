#include <ESP8266WiFi.h>
#include <ESP_Notify.h>

// input your values here
//############################################################
#define WIFI_SSID     "eslam111"   //eslam
#define WIFI_PASSWORD "11112222"  //a1234567

// Device ID you can get it from ESP Notify APP 
#define DEVICE_ID "fFU3to3WBAM:APA91bGwWeNJ9kh15_WHE-CkOzlNK03X6Vk4EHEUkI8mOVcbWJrtNXrf6Sb0IdbPAL1SI_f8Cd-sqxIETLeNEbt8hnWAZ-JZSubX5hYz9diLdmM6Ts640flMIssbFls5AzOOif6yOeFr"
//############################################################

// define a notifier
ESP_Notify notifier;


String message = "nope"; 

void setup() {
  Serial.begin(115200);
  // connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("\n\nconnecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.print("\nconnected: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  //read a meesage from Serial 
  if(Serial.available())
  {
    message = Serial.readString(); 
    delay(10);
  }

  //Send the message to your phone 
  if(message != "nope")   
  {
   if(message != "nope")
   {
     notifier.sendNotification(DEVICE_ID, message,"");
   }
   message = "nope";
  }
}
