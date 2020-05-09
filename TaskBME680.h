/* Ethernet Manager
 * This file deals with the CCS811 environmental sensor
 */
#ifndef __BME_680_H
#define __BME_680_H
#include "EthManager.h"

class TaskBME680
{
  public:

  /* Constructor */
  TaskBME680();

  /* Begin function (Spawns a new task) */
  void begin(void);
  void begin(float readInterval);

  /* Get most recent temp and humidity */
  float getTemp(void);
  float getHumid(void);

  /* Private data */
  
  /* Read Interval in seconds */
  float m_readInterval;
  /* Last recorded temp and humidity */
  float m_Temp;
  float m_Humid;

  void status(void);
  void stateLoad(void);
  void stateSave(void);
};

/* Global sensor object */
extern class TaskBME680 BME;


#endif
