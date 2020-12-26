#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <FirebaseArduino.h>
#include <ArduinoJson.h>
#include <DHT.h>

#define FIREBASE_HOST "iot-smartagriculture-project3-default-rtdb.firebaseio.com" //--> URL address of your Firebase Realtime Database. without http:// or https://  https://iot-smartagriculture-project3-default-rtdb.firebaseio.com/
#define FIREBASE_AUTH "KzLgfW7Wp5sTSQqdWqeQdfooO3OSHTPvjtWW2ETG" //--> Your firebase database secret code. Setting -> Service accounts -> Database secrets 
#define MSG_BUFFER_SIZE  (50)
// Update these with values suitable for your network.

const int typeDHT = DHT22;
const char* ssid = "HAI";
const char* password = "11111111";
const char* mqtt_server = "broker.hivemq.com";
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 3600;

unsigned long lastMsg = 0;
char msg[MSG_BUFFER_SIZE];
int value = 0;
int digSoilPin = 4; // GPIO4 ~ D2
int anaSoilPin = A0;
int DHT22Pin = 5; // GPIO5 ~ D1
int relayPumpPin = 2; // GPIO2 ~ D4
int relaySprayPin = 0; // GPIO0 ~ D3
int redLedPin =  12  ;    // GPIO ~ D6
int greenLedPin = 13  ;      // GPIO ~ D7

WiFiUDP ntpUDP;                    // giao thuc UDP ket noi qua wifi
NTPClient timeClient(ntpUDP, "pool.ntp.org");       // bien lay thoi gian tu internet
WiFiClient espClient;


PubSubClient client(espClient);
DHT dht(DHT22Pin, typeDHT);

//int relayPin = 4;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  digitalWrite(redLedPin, LOW);
  digitalWrite(greenLedPin, HIGH);

}

void callback(char* topic, byte* payload, unsigned int length) {
  String command = "";
  String stt ;
  String  a ;
  Serial.print(" Message arrived ["); // \a phat ra tieng keu
  Serial.print(topic);
  Serial.print("]:  ");
  for (int i = 0; i < length; i++) {
    command += (char)payload[i];
  }
  Serial.println(command);

  if ( strstr( command.c_str(),"spray") != NULL ){
    stt = strchr(command.c_str(), '=');
    if ( stt == "=1"){                                                            // Turn ON Spray
      digitalWrite(relaySprayPin, HIGH);
      Serial.println("=== Turn on Spray Machine == ");
    } else if ( stt == "=2"){                                                     // Turn OFF Spray
      digitalWrite(relaySprayPin, LOW);
      Serial.println("=== Turn off Spray Machine == ");
    }
  };

  if ( strstr( command.c_str(),"pump") != NULL ){
    stt = strchr( command.c_str(), '=');
    if ( stt == "=1"){                                                             // Turn ON Pump
      digitalWrite(relayPumpPin, HIGH);
      Serial.println("=== Turn on Pump Machine == ");
    } else if ( stt == "=2"){                                                     // Turn OFF Pump
      digitalWrite(relayPumpPin, LOW);
      Serial.println("=== Turn off Pump Machine == ");
    }
  };

}

void reconnect() {
  // Loop until we're reconnected
  digitalWrite(redLedPin, HIGH);
  digitalWrite(greenLedPin, LOW);
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266HaiClient-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
//      client.publish("testHaiTopic", "hello world");
      // ... and resubscribe
      client.subscribe("IOTHaiTayTuuTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  digitalWrite(redLedPin, LOW);
  digitalWrite(greenLedPin, HIGH);
  }
}

void setup() {
  
  pinMode(digSoilPin, INPUT);
  pinMode(anaSoilPin, INPUT);
  pinMode(relaySprayPin, OUTPUT);
  pinMode(relayPumpPin, OUTPUT);
  pinMode(redLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);

  digitalWrite(redLedPin, HIGH);
  digitalWrite(greenLedPin, LOW);
  dht.begin();
  Serial.begin(115200);
  setup_wifi();
  
  // get time from Time Server
  timeClient.begin(); 
  timeClient.setTimeOffset(+7*60*60); // set khu vuc mui gio cua VN
  //----------------------------------------Firebase Realtime Database Configuration.
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  //----------------------------------------
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}
void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();
  if (now - lastMsg > 60000) {
    lastMsg = now;

    int value = analogRead(anaSoilPin);
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    int sttPump = 0;
    int sttSpray = 0;
    int valSoil = 100 - map ( value, 0, 1023, 0, 100);
    
    Serial.print("Analog Do am dat: ");
  //  Serial.print(value);
    Serial.print(" ~ ");
    Serial.print(valSoil);
    Serial.println("%");
  
//    Serial.print("Digital Do am dat: "); 
//    Serial.println(digitalRead(digSoilPin));  // 0 la co nuoc ( đèn hiệu trên Sensor sáng )------ 1 la khong co nuoc
  
    Serial.print("Nhiet do: ");
    Serial.println(t);
    Serial.print("Do am khong khi: ");
    Serial.println(h);
  
    Serial.println("------------------------");

    ++value;
    snprintf (msg, MSG_BUFFER_SIZE, "soil=%d,temp=%0.2f,hum=%0.2f", valSoil, t, h);
    Serial.print("Publish message: ");
    Serial.println(msg);
//    client.publish("testHaiTopic", msg);
    Serial.println("================================================================");
    
    // get Date from Time Server
    timeClient.update();
    unsigned long epochTime = timeClient.getEpochTime();
    String formattedTime = timeClient.getFormattedTime();
    //Get a time structure
    struct tm *ptm = gmtime ((time_t *)&epochTime); 
  
    int monthDay = ptm->tm_mday;
  
    int currentMonth = ptm->tm_mon+1;
  
    int currentYear = ptm->tm_year+1900;
  
    //Print complete date:
    String currentDate = String(currentYear) + "-" + String(currentMonth) + "-" + String(monthDay);

    String fullTimeStamp = currentDate + " " + formattedTime;
    Serial.print("Current date: ");
    Serial.println(fullTimeStamp);
    
    
    Firebase.setString("Data/"+fullTimeStamp+"/HumiOS",String(valSoil)+"%"); //--> Command or code to send data or update data (String data type) to the Firebase Realtime Database.
    Firebase.setString("Data/"+fullTimeStamp+"/Temp",String(t)+"ºC"); //--> Command or code to send data or update data (String data type) to the Firebase Realtime Database.
    Firebase.setString("Data/"+fullTimeStamp+"/Humi",String(h)+"%"); //--> Command or code to send data or update data (String data type) to the Firebase Realtime Database.

//    sttPump = Firebase.setInt("Status/Pump");
//    sttSpray = Firebase.setInt("Status/Spray");
  // Conditions for handling errors.
    if (Firebase.failed()) { 
      Serial.print("Setting /Value failed :");
      Serial.println(Firebase.error());  
      delay(500);
      return;
    }
  
  }

  
}
