#include <WiFi.h>
#include <HttpClient.h>
#include <DHT.h>

const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

const char* influxDBUrl = "http://your-influxdb-url.com/write?db=your-database-name";

const int relayPins[] = {2, 3, 4, 5};
const int sensorPins[] = {A0, A1, A2, A3};
const int numSensors = sizeof(sensorPins) / sizeof(sensorPins[0]);
const int dhtPin = 6; // Pin connected to the DHT22 sensor
const int readingDelay = 2000;

WiFiClient client;
DHT dht(dhtPin, DHT22);

void setup()
{
  Serial.begin(9600);
  connectToWiFi();
  dht.begin();
  
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
  float ambientTemperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  
  if (isnan(ambientTemperature) || isnan(humidity))
  {
    Serial.println("Failed to read from DHT22 sensor!");
  }
  else
  {
    Serial.print("Ambient Temperature: ");
    Serial.print(ambientTemperature);
    Serial.print(" °C\tHumidity: ");
    Serial.print(humidity);
    Serial.println(" %");

    sendDataToInfluxDB("ambient", ambientTemperature);
    sendDataToInfluxDB("humidity", humidity);
  }

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

    sendDataToInfluxDB("moisture", outputValue, i);

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

void sendDataToInfluxDB(const char* measurement, float value, int sensorIndex = -1)
{
  String dataPoint;
  
  if (sensorIndex == -1)
  {
    dataPoint = String(measurement) + " value=" + String(value);
  }
  else
  {
    dataPoint = String(measurement) + ",sensor=" + String(sensorIndex) + " value=" + String(value);
  }

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