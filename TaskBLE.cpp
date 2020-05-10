/* This file deals with the Bluetooth Low Energy scanner */
#include "EthManager.h"
#include "TaskBLE.h"
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

#ifdef SENSOR_BLE

/* Global object */
class TaskBLE BLE;
BLEScan* pBLEScan;

/* Task to perform BLE scanning */
void TaskBLE_Poll(void *pvParameters)
{
  (void) pvParameters;
  Serial.println("BLE: Task Starting");
  TickType_t xLastWakeTime = xTaskGetTickCount();

  /* Initialize BLE */
  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setActiveScan(BLE.m_Active);
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);

  /* Loop forever */
  do
  {
    Serial.println("BLE: Performing Scan");
    BLEScanResults DevFound = pBLEScan->start(BLE.m_scanLength,false);
    int ResCount = DevFound.getCount();
    for(int i = 0; i < ResCount; i++)
    {
      BLEAdvertisedDevice ThisDev = DevFound.getDevice(i);
      const char *Name = ThisDev.getAddress().toString().c_str();
      static char Topic[128];
      sprintf(Topic,"net/ble/esp-%s/%s",Network.name(),Name);
      String output = "{";
      output += "\"RSSI\": "+String(ThisDev.getRSSI());
      if(ThisDev.haveTXPower()) output += ",\"TXPower\": "+String(ThisDev.getTXPower());
      output += "}";
      Serial.print("BLE: Topic ");
      Serial.print(Topic);
      Serial.print(" Data: ");
      Serial.println(output.c_str());
    }
    
    /* Wait remainder of delay */
    vTaskDelayUntil(&xLastWakeTime,((BLE.m_readInterval-BLE.m_scanLength) * configTICK_RATE_HZ));
  } while(1);

}
  

/* Constructors - set read interval to 5s by default */
TaskBLE::TaskBLE()
{
  m_readInterval = 5.0f;
  m_scanLength = 1.0f;
  m_Active = false;
}

/* Begin function */
void TaskBLE::begin(float readInterval,float scanLength,bool Active)
{
  m_readInterval = readInterval;
  m_Active = Active;
  m_scanLength = scanLength;
  begin();
}
void TaskBLE::begin(void)
{  
  /* Start scanner task */
  xTaskCreatePinnedToCore(
    TaskBLE_Poll,
    "TaskBLE",
    8192,
    NULL,
    0,
    NULL,
    ARDUINO_RUNNING_CORE);

}

/* SENSOR_BLE */
#endif 
