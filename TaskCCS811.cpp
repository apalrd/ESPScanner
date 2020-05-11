/* Ethernet Manager
 * This file deals with the global Ethernet and MQTT state
 */

#include "EthManager.h"
#include "Adafruit_CCS811.h"
#include "TaskCCS811.h"
#include "Wire.h"
#include "TaskBME680.h"

#ifdef SENSOR_CCS811

/* CCS811 object */
static Adafruit_CCS811 LocalCCS;

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
      /* Update environmental data from BME680 if equiped */
#ifdef SENSOR_BME680
      LocalCCS.setEnvironmentalData(
        BME.getHumid(),
        BME.getTemp());      
/* SENSOR_BME680 */
#endif
      /* Read data */
      if(LocalCCS.readData())
      {
        /* Error returned */
        Serial.println("CCS811: Error returned!");
      }
      else
      {
        /* Format MQTT response */
        sprintf(Topic,"raw/esp-%s/ccs811",Network.name().c_str());
        sprintf(Data,"{\"CO2\": %d,\"VOC\": %d}",LocalCCS.geteCO2(),LocalCCS.getTVOC());
        if(MQTT.connected())
        {
          Network.lock();
          MQTT.publish(Topic,Data,true);
          Network.unlock();
        }
        Serial.print("CCS811: ");
        Serial.println(Data);
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
      4096,
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

/* SENSOR_CCS811 */
#endif 
