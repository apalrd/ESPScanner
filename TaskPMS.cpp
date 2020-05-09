/* This file deals with the Plantower PMS family PM2.5 sensors */
#include "EthManager.h"
#include "TaskPMS.h"
#include "PMS_Sense.h"

#ifdef SENSOR_PMS

/* PMS object */
static PMSSense LocalPMS(Serial1);

/* Global CCS object */
class TaskPMS PMS;
PMSSense::DATA LocalData;

/* Task to poll sensor */
void TaskPMS_Poll(void *pvParameters)
{
  (void) pvParameters;
  Serial.println("PMS: Task Starting");
  TickType_t xLastWakeTime = xTaskGetTickCount();
  static char Topic[128];
  static String Data;

  /* Switch sensor to passive mode */
  LocalPMS.passiveMode();

  /* Loop forever */
  do
  {
    /* Wake up sensor and wait 30s for it to collect data  */
    Serial.println("PMS: Waking sensor up to collect readings");
    LocalPMS.wakeUp();
    vTaskDelay(30*configTICK_RATE_HZ);

    /* Send read request */
    Serial.println("PMS: Requesting read");
    LocalPMS.requestRead();
    /* Wait a bit for the sensor to reply */
    vTaskDelay(2*configTICK_RATE_HZ);
    /* Poll the sensor */
    if(LocalPMS.readUntil(LocalData))
    {
      /* Format MQTT response */
      sprintf(Topic,"raw/esp-%s/pms",Network.name().c_str());
      Data = "{";
      /* Atmospheric Environment */
      Data += "\"AE1.0\": "+String(LocalData.PM_AE_UG_1_0);
      Data += ", \"AE2.5\": "+String(LocalData.PM_AE_UG_2_5);
      Data += ", \"AE10\": "+String(LocalData.PM_AE_UG_10_0);
      Data += "}";
      if(MQTT.connected())
      {
        MQTT.publish_P(Topic,Data.c_str(),true);
      }
      Serial.print("PMS: ");
      Serial.println(Data);
    }
    else
    {
      Serial.println("PMS: Returned error");
    }

    /* Put the sensor back to sleep */
    LocalPMS.sleep();
    Serial.println("PMS: Sleeping");

    /* Delay by polling interval */
    vTaskDelayUntil(&xLastWakeTime,(PMS.m_readInterval * configTICK_RATE_HZ));
  } while(1);

}
  

/* Constructors - set read interval to 5s by default */
TaskPMS::TaskPMS()
{
  m_readInterval = 5.0f;
}

/* Begin function */
void TaskPMS::begin(float readInterval)
{
  m_readInterval = readInterval;
  begin();
}
void TaskPMS::begin(void)
{
  /* Start Serial1 */
  Serial1.begin(9600,SERIAL_8N1,36,34);

  /* Wait 2 seconds for data */
  delay(2000);
  
  /* Start sensor */
  xTaskCreatePinnedToCore(
    TaskPMS_Poll,
    "TaskPMS",
    4096,
    NULL,
    1,
    NULL,
    ARDUINO_RUNNING_CORE);

}

/* SENSOR_PMS */
#endif 
