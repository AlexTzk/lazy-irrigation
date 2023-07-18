const int relayPins[] = {2, 3, 4, 5};
const int sensorPins[] = {A0, A1, A2, A3};
const int numSensors = sizeof(sensorPins) / sizeof(sensorPins[0]);
const int readingDelay = 2000;

void setup()
{
  Serial.begin(9600);
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
