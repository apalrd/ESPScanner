/* Ethernet Manager
 * This file deals with the global Ethernet and MQTT state
 */
#include <ETH.h>
#include "EthManager.h"
#include <cctype>

/* Global instance */
class EthManager Network;

/* Global WiFiClient */
WiFiClient client;

/* Global MQTT instance */
PubSubClient MQTT(client);

/* Callback function for Ethernet (Not a class member */
void EthManagerCallback(WiFiEvent_t event)
{
  const char *CMAC;
  char NMAC[18];
  String hName;
  switch (event) {
    case SYSTEM_EVENT_ETH_START:
      Serial.print("ETH: Started.. ");
      Serial.print("MAC: ");
      Serial.println(ETH.macAddress());
      /* Generate 6-character device name */
      CMAC = ETH.macAddress().c_str();
      for(int i = 0; i < strlen(CMAC);i++)
      {
        NMAC[i] = (char)tolower(CMAC[i]);
      }
      sprintf(NMAC,"%c%c%c%c%c%c",
              tolower(CMAC[9]),tolower(CMAC[10]),
              tolower(CMAC[12]),tolower(CMAC[13]),
              tolower(CMAC[15]),tolower(CMAC[16]));
      Network.m_Name = NMAC;
      hName = "esp-"+Network.m_Name;
      Serial.print(" Hostname: ");
      Serial.println(hName);
      //set eth hostname here
      ETH.setHostname(hName.c_str());
      break;
    case SYSTEM_EVENT_ETH_CONNECTED:
      Serial.println("ETH: Connected");
      break;
    case SYSTEM_EVENT_ETH_GOT_IP:
      Serial.print("ETH: Got IPv4 ");
      Serial.print(ETH.localIP());
      if (ETH.fullDuplex()) {
        Serial.print(", FULL_DUPLEX");
      }
      Serial.print(", ");
      Serial.print(ETH.linkSpeed());
      Serial.println("Mbps");
      Network.m_Connected = true;
      break;
    case SYSTEM_EVENT_ETH_DISCONNECTED:
      Serial.println("ETH: Disconnected");
      Network.m_Connected = false;
      break;
    case SYSTEM_EVENT_ETH_STOP:
      Serial.println("ETH: Stopped");
      Network.m_Connected = false;
      break;
    default:
      break;
  }
}

/* Constructor */
EthManager::EthManager()
{
  /* Initially not connected */
  m_Connected = false;
}


/* Begin function */
void EthManager::begin(void)
{
  /* Initialize */
  WiFi.onEvent(EthManagerCallback);
  ETH.begin();

  /* Print FreeRTOS data */
  Serial.print("Arduino Running on Core: ");
  Serial.println(ARDUINO_RUNNING_CORE);
  Serial.print("FreeRTOS Tick Hz: ");
  Serial.println(configTICK_RATE_HZ);
  Serial.print("FreeRTOS Tick Period Ms: ");
  Serial.println(portTICK_PERIOD_MS);  

  /* Setup MQTT */
  MQTT.setServer(MQTT_SERVER,MQTT_PORT);
}

/* Loop function */
void EthManager::loop(void)
{
  /* Check if MQTT is connected */
  if(!MQTT.connected())
  {
    /* Print log info */
    Serial.print("MQTT: Attempting Reconnection (ID ");
    String clientID = "esp-"+m_Name;
    Serial.print(clientID);
    Serial.println(')');
    /* Attempt connection */
    if(MQTT.connect(clientID.c_str()))
    {
      /* Connected! */
      Serial.println("MQTT: Connected");
    }
    else
    {
      /* Failed :( */
      Serial.println("MQTT: Connection Failed");
      delay(MQTT_RECONNECT_TIME);
    }
  }
  else
  {
    /* MQTT is connected, just sit tight */
    delay(5000);
  }
}

/* Get connection status */
bool EthManager::connected(void)
{
  return m_Connected;
}

/* Get local name string */
String EthManager::name(void)
{
  return m_Name;
}
