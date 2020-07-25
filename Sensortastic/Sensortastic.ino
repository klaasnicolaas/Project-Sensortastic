// Libraries or Packages import
#include "config.h"
#include <ESP8266WiFi.h>
#include <WifiClient.h>
#include <ESP8266WebServer.h>
#include <Scheduler.h>
#include <InfluxDbClient.h>

// DHT Sensor Library
#include "DHT.h"
#define DHTTYPE DHT22                   //--> Defines the type of DHT sensor used (DHT11, DHT21, and DHT22).

// Webpage
#include "PageIndex.h"                  //--> Include the contents of the User Interface Web page, stored in the same folder as the .ino file

// LED on board
#define LEDonBoard 2                    //--> Defining an On Board LED, used for indicators when the process of connecting to a wifi router

ESP8266WebServer server(80);            //--> Server on port 80

const int DHTPin = 5;                   //--> The pin used for the DHT22 sensor is Pin D1=Pin 5
DHT dht(DHTPin, DHTTYPE);               //--> Initialize DHT sensor, DHT dht(Pin_used, Type_of_DHT_Sensor);

// Single InfluxDB instance
InfluxDBClient client(INFLUXDB_URL, INFLUXDB_DB_NAME);

// Data point
Point sensor(INFLUXDB_MEASUREMENT);

// InfluxDB
class InfluxdbUploadTask : public Task {
  public:
    void sendInfluxDB() {
      float tempValue;
      float humidValue;
      tempValue = getTemp();
      humidValue = getHumid();

      sensor.clearFields();

      // Log in Serial monitor
      Serial.print("READING: DHT22 || Temperature : ");
      Serial.print(tempValue);
      Serial.print(" || ");
      Serial.print("Humidity: ");
      Serial.println(humidValue);

      if(WiFi.status() == WL_CONNECTED) {
        // Set InfluxDB 1 authentication params
        client.setConnectionParamsV1(INFLUXDB_URL, INFLUXDB_DB_NAME, INFLUXDB_USER, INFLUXDB_PASSWORD);
        sensor.addField("temperature", tempValue);
        sensor.addField("humidity", humidValue);

        // Write point
        if (client.writePoint(sensor)) {
          Serial.println("INFO: InfluxDB upload succesfull");
          
          if (uploadLedPulse ==  true) {
            // Blink LED if upload was succesfull
            Serial.println("INFO: Time to blink the led!");
            digitalWrite(LEDonBoard, LOW);
            delay(250);
            digitalWrite(LEDonBoard, HIGH);
            delay(250);
          }
        } else {
          Serial.println();
          Serial.println((String)"InfluxDB write failed: " + client.getLastErrorMessage());
        }
      }
    }

    // Function for getting temperature
    float getTemp() {
      float result;
      result = dht.readTemperature(); 
      return result;
    }

    // Function for getting humidity
    float getHumid() {
      float result;
      result = dht.readHumidity();
      return result;
    }
  protected:
    void setup() {
      // Add static value tags
      sensor.addTag("location", LOCATION);
    }
    void loop() {
      sendInfluxDB();
      delay(waitTime);
    }
} influxdb_upload;

// Keep servers up and running
class HandleServers : public Task {
  protected:
    void setup() {
      server.begin();                                       //--> Start server
      Serial.println("INFO: HTTP server started");
    }

    void loop() {
      server.handleClient();
    }
} handle_servers;

///////////////////////////////////////////////////////////////////////////
//   Webserver 
///////////////////////////////////////////////////////////////////////////
void handleRoot() {
  server.send(200, "text/html", MAIN_page);     //--> Send web page
}

///////////////////////////////////////////////////////////////////////////
//   Read DHT22 Temperature 
///////////////////////////////////////////////////////////////////////////
void handleDHT22Temperature() {
  float t = dht.readTemperature();                    //--> Read temperature as Celsius (the default).
  // float t = dht.readTemperature(true);             //--> When using Fahrenheit 
  String TemperatureValue = String(t);

  server.send(200, "text/plane", TemperatureValue);   //--> Send Temperature value only to client ajax request

  if (isnan(t)) {                                     //--> Check if any reads failed and exit early (to try again).
    Serial.println("ERROR: Failed to read from DHT sensor!");
  }
}

///////////////////////////////////////////////////////////////////////////
//   Read DHT22 Humidity
///////////////////////////////////////////////////////////////////////////
void handleDHT22Humidity() {
  float h = dht.readHumidity();
  String HumidityValue = String(h);

  server.send(200, "text/plane", HumidityValue);      //--> Send Humidity value only to client ajax request

  if (isnan(h)) {
    Serial.println("ERROR: Failed to read from DHT sensor!");
  }
}

///////////////////////////////////////////////////////////////////////////
//   SETUP() & LOOP()
///////////////////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(115200);
  delay(500);
  dht.begin();                            //--> Start reading DHT22 sensor
  delay(500);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);   //--> Connect to your WiFi router
  Serial.println("");
  
  pinMode(LEDonBoard,OUTPUT);             //--> On Board LED port Direction output
  digitalWrite(LEDonBoard, HIGH);         //--> Turn off Led On Board

  // Wait for connection
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");

    // Make the On Board Flashing LED on the process of connecting to the wifi router. 
    digitalWrite(LEDonBoard, LOW);
    delay(250);
    digitalWrite(LEDonBoard, HIGH);
    delay(250);
  }

  digitalWrite(LEDonBoard, HIGH);         //--> Turn off the On Board LED when it is connected to the wifi router.

  // If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.print("INFO: Succesfully connected to: ");
  Serial.println(WIFI_SSID);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);                           //--> Routine to handle at root location. This is to display web page.
  server.on("/temperature", handleDHT22Temperature);    //--> Routine to handle the call procedure handleDHT22Temperature
  server.on("/humidity", handleDHT22Humidity);          //--> Routine to handle the call procedure handleDHT22Humidity

  Scheduler.start(&handle_servers);
  Scheduler.start(&influxdb_upload);
  Scheduler.begin();

}

void loop() {}
