/*
  Titre      : MQTTConnector.h
  Auteur     : Cedric Yonta
  Date       : 05/11/2021
  Description: Utilitaire de branchement a un Broker MQTT une fois une connexion établisa l'internet via WIFI, GSM, etc...
  Droits     : Reproduction permise pour usage pédagogique
  Version    : 0.0.1
  source : Andre Roussel
*/

#include <MQTT.h>

// Constante nécessaire pour le branchement au Broker MQTT

const char MQTT_SERVER[] = SECRET_MQTT_SERVER_IP;     // Adresse IP du broker MQTT
const int MQTT_SERVER_PORT = SECRET_MQTT_SERVER_PORT; // Port du broker MQTT

// Pramètre d'identification de l'objet
const char TOKEN[] = SECRET_TOKEN;         // Token d'accès de l'objet
const char DEVICE_ID[] = SECRET_DEVICE_ID; // Numéro d'indentification de l'objet provenant de Thingsboard

MQTTClient ClientMQTT; // Création d'un client MQTT pour l'échange de donnée entre l'objet IDO et le broker MQTT

String Payload = "{"; // Chaine de caractère qui contiendra le message envoyer de l'objet vers thingsboard
String valStatus;


void messageReceived(String &topic, String &payload)
{

  Serial.println("Message Recu");
  Serial.println(payload);
  Serial.println(topic);
 
  Serial.println(payload.substring(11, 12));
  Serial.println(payload.substring(28, 29));
  valStatus = payload.substring(29, 30);
  Serial.println("Status");

  /*if (payload.substring(29, 30) == "h")
  {
    Serial.println("BLUE_PIN High ");
    servoPos = 180;
    LedStatus = true;
    digitalWrite(BLUE_PIN, HIGH); 
    myservo.write(servoPos);
    // faire le choix sur la led qui s'allume
  }
 
   else if (payload.substring(29, 30) == "b")
  {
    Serial.println("RED_PIN High ");
     servoPos = 0;
    LedStatus = true;
    digitalWrite(LED_PIN, HIGH);
    myservo.write(servoPos);
  }
   else if(payload.substring(28, 29) == "f")
  {
     Serial.println("PIN Low ");
     LedStatus = false;
     myservo.write(servoPos);
  }else
  {
    /* code */
 // }
  
  
}

// Fonctionnalité de branchement utilisant le protocole MQTT

void MQTTConnect()
{

  ClientMQTT.begin(MQTT_SERVER, MQTT_SERVER_PORT, ClientWIFI);

  ClientMQTT.onMessage(messageReceived);

  while (!ClientMQTT.connect(DEVICE_ID, TOKEN, ""))
  {
    Serial.print(".");
    delay(1000);
  }

  // traitement des message recu

  Serial.println("\nBranché au broker MQTT!\n");
  delay(1000);

  ClientMQTT.subscribe("v1/devices/me/rpc/request/+");
}


void sendMQTTStringAsPayload(String myPayload)
{
  char myAttributes[200];

  myPayload.toCharArray(myAttributes, 200);
  ClientMQTT.publish("v1/devices/me/telemetry", myAttributes);

  Serial.print("Payload -> ");
  Serial.println(Payload);
  Serial.print("Payload length -> ");
  Serial.println(Payload.length());
}
