/* Ethernet Manager
 * This file deals with the global Ethernet and MQTT state
 */
#include <ETH.h>
#include "EthManager.h"

/* Global instance */
class EthManager Network;

/* Callback function (Not a class member */
void EthManagerCallback(WiFiEvent_t event)
{
  switch (event) {
    case SYSTEM_EVENT_ETH_START:
      Serial.println("ETH Started");
      Serial.print("ETH MAC: ");
      Serial.println(ETH.macAddress());
      //set eth hostname here
      ETH.setHostname("esp32-ethernet");
      break;
    case SYSTEM_EVENT_ETH_CONNECTED:
      Serial.println("ETH Connected");
      break;
    case SYSTEM_EVENT_ETH_GOT_IP:
      Serial.print("ETH MAC: ");
      Serial.print(ETH.macAddress());
      Serial.print(", IPv4: ");
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
      Serial.println("ETH Disconnected");
      Network.m_Connected = false;
      break;
    case SYSTEM_EVENT_ETH_STOP:
      Serial.println("ETH Stopped");
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
