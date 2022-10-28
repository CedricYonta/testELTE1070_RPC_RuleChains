/*
  Titre      : Controle à distance via RPC
  Auteur     : Cedric Yonta
  Date       : 28/10/2022
  Description: Une description du programme
  Version    : 0.0.1
*/

#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_AHTX0.h>
#include "RTClib.h"
#include <Servo.h>
#include "WIFIConnector_MKR1000.h"
#include "MQTTConnector.h"

// instantation des classes 
RTC_DS3231 rtc;
Adafruit_AHTX0 aht;
Servo myservo;

// Declartions des variables
String buffer;
String valHumidite, valTemperature, unixTime = "";
const int CHIPSELECT = 4;
int espace1, espace2 = 0;
char charToWrite[80] = {0};
const int LED_PIN = 3;
const int BLUE_PIN = 2;
int servoPos = 90;
unsigned int const fileWriteTime = 5000;
unsigned int const timeToSend = 30000;
unsigned int const endWifi = 30000;

// fonctions pour delais 
boolean checkTimeShort(unsigned long interval)

{
    static unsigned long previousMillis = 0;
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval)
    {
        previousMillis = currentMillis;

        return true;
    }

    return false;
}

boolean checkTimeLong(unsigned long interval)

{
    static unsigned long previousMillis = 0;

    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= interval)
    {
        previousMillis = currentMillis;

        return true;
    }

    return false;
}

void setup()
{
    // Branchement au reseau
    wifiConnect(); // Branchement au réseau WIFI
    MQTTConnect(); // Branchement au broker MQTT à Thingsboard

    // pin servo moteur
    myservo.attach(6);

    // Open serial communications and wait for port to open:
    Serial.begin(9600);
    /*while (!Serial)
    {
      ; // wait for serial port to connect. Needed for native USB port only
    }*/

    // see if the card is present and can be initialized:
    if (!SD.begin(CHIPSELECT))
    {
        Serial.println("Card failed, or not present");
        // don't do anything more:
        while (1)
            ;
    }

    // Init de AHT20
    if (!aht.begin())
    {
        Serial.println("Impossible de trouver l'AHT ? Vérifiez le câblage");
        while (1)
            delay(10);
    }
    Serial.println(" AHT20 trouve");

    // Init du RTC
    if (!rtc.begin())
    {
        Serial.println("Couldn't find RTC");
        Serial.flush();
        while (1)
            delay(10);
    }

    if (rtc.lostPower())
    {
        Serial.println("RTC lost power, let's set the time!");
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }

    File dataFile = SD.open("datalog.txt", FILE_WRITE | O_TRUNC);
    dataFile.close();
}

void loop()
{
    
    // capture de données
    DateTime now = rtc.now();
    sensors_event_t humidity, temp;
    aht.getEvent(&humidity, &temp);

   // chaine à ecrire dans la carteSD
    sprintf(charToWrite, "{\"ts\":%.0f,\"values\":{\"Temperature\":%.2f,\"Humidite\":%.2f}}", (double)now.unixtime() * 1000, temp.temperature, humidity.relative_humidity);


     // ecriture dans la carte SD
    if (checkTimeShort(fileWriteTime))
    {
        File dataFile = SD.open("datalog.txt", FILE_WRITE);
        // if the file is available, write to it:
        if (dataFile)
        {

            dataFile.println(charToWrite);
            dataFile.close();
        }
        else
        {
            Serial.println("ouverture d'erreur datalog.txt");
        }
    }

    // envoi sur thingsboards
    if (checkTimeLong(timeToSend))
    {
        wifiConnect(); // Branchement au réseau WIFI
        MQTTConnect();

        File dataFile = SD.open("datalog.txt", FILE_READ);
        if (dataFile)
        {
            // read from the file until there's nothing else in it:
            while (dataFile.available())
            {
                Payload = dataFile.readStringUntil('\n');

                sendMQTTStringAsPayload(Payload);
                ClientMQTT.loop();

               /*if (valStatus == "h")
                {
                    Serial.println("Action clim");
                    servoPos = 180;
                    digitalWrite(LED_PIN, HIGH);
                    myservo.write(servoPos);
                    // faire le choix sur la led qui s'allume
                }

                else if (valStatus == "b")
                {
                    Serial.println("Action chauffage ");
                    servoPos = 0;
                    digitalWrite(BLUE_PIN, HIGH);
                    myservo.write(servoPos);
                }
                else if (valStatus == "f")
                {
                    Serial.println("PIN Low ");
                    digitalWrite(BLUE_PIN, LOW);
                    digitalWrite(LED_PIN, LOW);
                    myservo.write(servoPos);
                }
                else
                {
                    /* code */
                //}
                // attente avant de fermer le wifi
                /*unsigned int const afterSending = millis();
                if (millis() - afterSending >= endWifi)
                {
                   
                    status = WL_IDLE_STATUS;
                    WiFi.disconnect();
                    WiFi.end();
                    dataFile.close();
                    SD.remove("datalog.txt");
                }*/
            }

            
        }
        else
        {

            // if the file didn't open, print an error:
            Serial.println("ouverture d'erreur datalog.txt");
        }
    }

    if (valStatus == "h")
                {
                    Serial.println("Action clim");
                    servoPos = 180;
                    digitalWrite(LED_PIN, HIGH);
                    myservo.write(servoPos);
                    // faire le choix sur la led qui s'allume
                }

                else if (valStatus == "b")
                {
                    Serial.println("Action chauffage ");
                    servoPos = 0;
                    digitalWrite(BLUE_PIN, HIGH);
                    myservo.write(servoPos);
                }
                else if (valStatus == "f")
                {
                    Serial.println("PIN Low ");
                    digitalWrite(BLUE_PIN, LOW);
                    digitalWrite(LED_PIN, LOW);
                    myservo.write(servoPos);
                }
                else
                {
                    /* code */
                }
}
