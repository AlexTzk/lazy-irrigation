#include <WiFi.h>
#include <HttpClient.h>

const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

const char* influxDBUrl = "http://your-influxdb-url.com/write?db=your-database-name";

const int relayPins[] = {2, 3, 4, 5};
const int sensorPins[] = {A0, A1, A2, A3};
const int numSensors = sizeof(sensorPins) / sizeof(sensorPins[0]);
const int readingDelay = 2000;

WiFiClient client;

void setup()
{
  Serial.begin(9600);
  connectToWiFi();
  for (int i = 0; i < numSensors; i++)
  {
    pinMode(relayPins[i], OUTPUT);
    pinMode(sensorPins[i], INPUT);
    Serial.print("Reading From Sensor ");
    Serial.print(i);
    Serial.println(" ...");
    delay(readingDelay);
  }
}

void loop()
{
  for (int i = 0; i < numSensors; i++)
  {
    int outputValue = analogRead(sensorPins[i]);
    Serial.print("Sensor ");
    Serial.print(i);
    Serial.print(" - Analog Moisture: ");
    Serial.println(outputValue);
    outputValue = map(outputValue, 550, 190, 0, 100);
    Serial.print("Sensor ");
    Serial.print(i);
    Serial.print(" - Moisture: ");
    Serial.print(outputValue);
    Serial.println("%");

    sendDataToInfluxDB(i, outputValue);

    if (outputValue < 20)
    {
      digitalWrite(relayPins[i], LOW);
    }
    else
    {
      digitalWrite(relayPins[i], HIGH);
    }
    
    delay(1000);
  }
}

void connectToWiFi()
{
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected!");
}

void sendDataToInfluxDB(int sensorIndex, int moistureValue)
{
  String dataPoint = "moisture,sensor=" + String(sensorIndex) + " value=" + String(moistureValue);

  if (client.connect(influxDBUrl))
  {
    client.println("POST " + String(influxDBUrl) + " HTTP/1.1");
    client.println("Host: your-influxdb-url.com");
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.print("Content-Length: ");
    client.println(dataPoint.length());
    client.println();
    client.println(dataPoint);
    client.println();
    Serial.println("Data sent to InfluxDB!");
  }
  else
  {
    Serial.println("Failed to connect to InfluxDB!");
  }

  client.stop();
}
