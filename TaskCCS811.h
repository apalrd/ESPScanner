/* Ethernet Manager
 * This file deals with the CCS811 environmental sensor
 */
#ifndef __CCS_811_H
#define __CCS_811_H
#include "EthManager.h"

class TaskCCS811
{
  public:

  /* Constructor */
  TaskCCS811();

  /* Begin function (Spawns a new task) */
  void begin(void);
  void begin(float readInterval);

  /* Private data */
  
  /* Read Interval in seconds */
  float m_readInterval;
};

/* Global CCS object */
extern class TaskCCS811 CCS;


#endif
