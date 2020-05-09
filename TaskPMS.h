/* This file deals with the Plantower PMS family PM2.5 sensors */
#ifndef __PMS_H
#define __PMS_H
#include "EthManager.h"

#ifdef SENSOR_PMS

class TaskPMS
{
  public:

  /* Constructor */
  TaskPMS();

  /* Begin function (Spawns a new task) */
  void begin(void);
  void begin(float readInterval);

  /* Private data */
  
  /* Read Interval in seconds */
  float m_readInterval;
};

/* Global sensor object */
extern class TaskPMS PMS;

/* SENSOR_PMS */
#endif

/* __PMS_H */
#endif
