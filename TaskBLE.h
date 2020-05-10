/* This file deals with the Bluetooth Low Energy scanner */
#ifndef __BLE_H
#define __BLE_H
#include "EthManager.h"

#ifdef SENSOR_BLE

class TaskBLE
{
  public:

  /* Constructor */
  TaskBLE();

  /* Begin function (Spawns a new task) */
  void begin(void);
  void begin(float readInterval,float scanLength,bool Active);

  /* Private data */
  
  /* Read Interval in seconds */
  float m_readInterval;
  float m_scanLength;
  bool m_Active;
};

/* Global sensor object */
extern class TaskBLE BLE;

/* SENSOR_BLE */
#endif

/* __BLE_H */
#endif
