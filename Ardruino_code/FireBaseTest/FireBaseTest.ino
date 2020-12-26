//####################################################################################################################################################################################
//---------------------------------------------------------------------------------Test Set and Get Data from Firebase Realtime Database
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <ArduinoJson.h>
//#include <ESP8266HTTPClient.h>

#define FIREBASE_HOST "iot-smartagriculture-project3-default-rtdb.firebaseio.com" //--> URL address of your Firebase Realtime Database. without http:// or https://  https://iot-smartagriculture-project3-default-rtdb.firebaseio.com/
#define FIREBASE_AUTH "KzLgfW7Wp5sTSQqdWqeQdfooO3OSHTPvjtWW2ETG" //--> Your firebase database secret code. Setting -> Service accounts -> Database secrets 

#define ON_Board_LED 2  //--> Defining an On Board LED, used for indicators when the process of connecting to a wifi router

//----------------------------------------SSID and Password of your WiFi router.
const char* ssid = "ThangLoi"; //--> Your wifi name or SSID.
const char* password = "thuthuy123"; //--> Your wifi password.
//----------------------------------------

long randNumber;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(500);
  
  WiFi.begin(ssid, password); //--> Connect to your WiFi router
  Serial.println("");

  //----------------------------------------Wait for connection
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
      delay(500);
  }
  Serial.println("");
  Serial.print("Successfully connected to : ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  //----------------------------------------

  //----------------------------------------Firebase Realtime Database Configuration.
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  //----------------------------------------
}

void loop() {
  // put your main code here, to run repeatedly:

  //----------------------------------------Send data in the form of random value data to the Firebase Realtime Database.
  randNumber = random(100, 1000);
  String dataSend = String(randNumber); 
  Serial.print("Random Number : ");
  Serial.println(dataSend);

  Firebase.setString("RandomVal/Value",dataSend); //--> Command or code to send data or update data (String data type) to the Firebase Realtime Database.

  // Conditions for handling errors.
  if (Firebase.failed()) { 
      Serial.print("Setting /Value failed :");
      Serial.println(Firebase.error());  
      delay(500);
      return;
  }

  Serial.println("Setting successful");
  Serial.println();
  //----------------------------------------

  //----------------------------------------Command or code to get data from Firebase Realtime Database.
  /*
  Serial.print("Value : ");
  Serial.println(Firebase.getString("RandomVal/Value")); //--> Command or code to get data (String data type) from Firebase Realtime Database.

  // Conditions for handling errors.
  if (Firebase.failed()) { 
      Serial.print("Getting /Value failed :");
      Serial.println(Firebase.error()); 
      delay(500); 
      return;
  }
  
  //----------------------------------------
  */
  delay(5000);
  
  // For other examples of FirebaseArduino, look in File -> Examples -> FirebaseArduino.
}
