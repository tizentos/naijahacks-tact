#include <PubSubClient.h>
#include <ESP8266WiFi.h>

#define WIFI_AP "YOUR_PHONE_HOTSPOT_NAME"
#define WIFI_PASSWORD "HOTSPOT_PASSWORD"


#define TOKEN "6xEP4uTx2fY5224VlliR"

char thingsboardServer[] = "demo.thingsboard.io";

WiFiClient wifiClient;

PubSubClient client(wifiClient);

int status = WL_IDLE_STATUS;
unsigned long lastSend;

//ECG input pin
#define BIOSENSOR_PIN 0    //change to NodeMCU ADC

void setup() {
  Serial.begin(115200);
  delay(10);
  InitWiFi();
  client.setServer( thingsboardServer, 1883 );
  lastSend = 0;
}

void loop() {
  if ( !client.connected() ) {
    reconnect();
  }

  if ( millis() - lastSend > 10 ) { // Update and send only after 1 seconds
    getAndSendBioSensorData();
    lastSend = millis();
  }

  client.loop();
}

void getAndSendBioSensorData(){
 int biosensorValue=analogRead(BIOSENSOR_PIN);
 Serial.println(biosensorValue);

  // Prepare a JSON payload string
  String payload = "{";
  payload += "\"ECGReading\":"; 
  payload += biosensorValue;
  payload += "}";

  // Send payload
  char attributes[100];
  payload.toCharArray( attributes, 100 );
  client.publish( "v1/devices/me/telemetry", attributes );
  Serial.println( attributes );
}

void InitWiFi() {
  Serial.println("Connecting to AP ...");
  // attempt to connect to WiFi network
  WiFi.begin(WIFI_AP, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to AP");
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    status = WiFi.status();
    if ( status != WL_CONNECTED) {
      WiFi.begin(WIFI_AP, WIFI_PASSWORD);
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }
      Serial.println("Connected to AP");
    }
    Serial.print("Connecting to ThingsBoard node ...");
    // Attempt to connect (clientId, username, password)
    if ( client.connect("ESP8266 Device", TOKEN, NULL) ) {
      Serial.println( "[DONE]" );
    } else {
      Serial.print( "[FAILED] [ rc = " );
      Serial.print( client.state() );
      Serial.println( " : retrying in 5 seconds]" );
      // Wait 5 seconds before retrying
      delay( 5000 );
    }
  }
}
