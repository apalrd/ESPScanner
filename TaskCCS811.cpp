/* Ethernet Manager
 * This file deals with the global Ethernet and MQTT state
 */

#include "EthManager.h"
#include "Adafruit_CCS811.h"
#include "TaskCCS811.h"
#include "Wire.h"


/* CCS811 object */
Adafruit_CCS811 LocalCCS;

/* Global CCS object */
class TaskCCS811 CCS;

/* Task to poll sensor */
void TaskCCS811_Poll(void *pvParameters)
{
  (void) pvParameters;
  Serial.println("CCS811: Task Starting");
  TickType_t xLastWakeTime = xTaskGetTickCount();
  static char Topic[128];
  static char Data[1024];

  /* Loop forever */
  do
  {
    /* Poll the sensor */
    if(LocalCCS.available())
    {
      /* Read data */
      if(LocalCCS.readData())
      {
        /* Error returned */
        Serial.println("CCS811: Error returned!");
      }
      else
      {
        /* No error returned */
        Serial.print("CCS811: Read data CO2: ");
        Serial.print(LocalCCS.geteCO2());
        Serial.print("ppm, TVOC: ");
        Serial.print(LocalCCS.getTVOC());
        Serial.println();

        /* Format MQTT response */
        sprintf(Topic,"raw/esp-%s/ccs811",Network.name().c_str());
        sprintf(Data,"{\"CO2\": %d,\"TVOC\": %d}",LocalCCS.geteCO2(),LocalCCS.getTVOC());
        if(MQTT.connected())
        {
          MQTT.publish(Topic,Data);
        }
      }
    }

    /* Delay by polling interval */
    vTaskDelayUntil(&xLastWakeTime,(CCS.m_readInterval * configTICK_RATE_HZ));
  } while(1);

}
  

/* Constructors - set read interval to 5s by default */
TaskCCS811::TaskCCS811()
{
  m_readInterval = 5.0f;
}

/* Begin function */
void TaskCCS811::begin(float readInterval)
{
  m_readInterval = readInterval;
  begin();
}
void TaskCCS811::begin(void)
{
  /* Start sensor */
  if(LocalCCS.begin())
  {
    /* Successfully started sensor, setup polling task */
    Serial.println("CCS811: Sensor Initialized");
    xTaskCreatePinnedToCore(
      TaskCCS811_Poll,
      "TaskCCS811",
      1024,
      NULL,
      2,
      NULL,
      ARDUINO_RUNNING_CORE);
  }
  else
  {
    /* Failed to start :( */
    Serial.println("CCS811: Failed to start sensor!");
  }
}
