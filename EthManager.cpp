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
  static String hName;
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
      Serial.print("ETH: Hostname ");
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

  /* Initialize networking mutex */
  m_EthLock = xSemaphoreCreateMutex();

  /* Setup MQTT */
  MQTT.setServer(MQTT_SERVER,MQTT_PORT);
}

String EthManager::ResetReason(RESET_REASON reason)
{
  switch ( reason)
  {
    case 1 : return "POWERON_RESET";break;          /**<1,  Vbat power on reset*/
    case 3 : return "SW_RESET";break;               /**<3,  Software reset digital core*/
    case 4 : return "OWDT_RESET";break;             /**<4,  Legacy watch dog reset digital core*/
    case 5 : return "DEEPSLEEP_RESET";break;        /**<5,  Deep Sleep reset digital core*/
    case 6 : return "SDIO_RESET";break;             /**<6,  Reset by SLC module, reset digital core*/
    case 7 : return "TG0WDT_SYS_RESET";break;       /**<7,  Timer Group0 Watch dog reset digital core*/
    case 8 : return "TG1WDT_SYS_RESET";break;       /**<8,  Timer Group1 Watch dog reset digital core*/
    case 9 : return "RTCWDT_SYS_RESET";break;       /**<9,  RTC Watch dog Reset digital core*/
    case 10 : return "INTRUSION_RESET";break;       /**<10, Instrusion tested to reset CPU*/
    case 11 : return "TGWDT_CPU_RESET";break;       /**<11, Time Group reset CPU*/
    case 12 : return "SW_CPU_RESET";break;          /**<12, Software reset CPU*/
    case 13 : return "RTCWDT_CPU_RESET";break;      /**<13, RTC Watch dog Reset CPU*/
    case 14 : return "EXT_CPU_RESET";break;         /**<14, for APP CPU, reseted by PRO CPU*/
    case 15 : return "RTCWDT_BROWN_OUT_RESET";break;/**<15, Reset when the vdd voltage is not stable*/
    case 16 : return "RTCWDT_RTC_RESET";break;      /**<16, RTC Watch dog reset digital core and rtc module*/
    default : return "NO_MEAN";
  }
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
    String WillTopic = "system/"+clientID;
    String WillMsg = "{\"Life\": -1}";
    /* Attempt connection */
    if(MQTT.connect(clientID.c_str(), /* Client ID */
                    WillTopic.c_str(),/* Will Topic */
                    1, true,          /* Will Qos and Retain */
                    WillMsg.c_str())) /* Will Message */
    {
      /* Connected! */
      Serial.println("MQTT: Connected");
      String LifeMsg = "{\"Life\":1";
      /* Add reset reason */
      LifeMsg += ",\"ResetReason0\":\""+ResetReason(rtc_get_reset_reason(0))+"\"";
      LifeMsg += ",\"ResetReason1\":\""+ResetReason(rtc_get_reset_reason(1))+"\"";
      LifeMsg += "}";
      MQTT.publish(WillTopic.c_str(),LifeMsg.c_str(),true);
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

/* Lock and Unlock the EthManager mutex */
void EthManager::lock(void)
{
  xSemaphoreTake(m_EthLock,portMAX_DELAY);
}
void EthManager::unlock(void)
{
  xSemaphoreGive(m_EthLock);
}
