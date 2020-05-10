/* Ethernet Manager
 * This file deals with the global Ethernet and MQTT state
 */
#ifndef __ETH_MANAGER_H
#define __ETH_MANAGER_H
#include <ETH.h>
#include <PubSubClient.h>
#include "Configuration.h"

#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

/* Global MQTT instance */
extern PubSubClient MQTT;

/* Callback function (Not a class member */
void EthManagerCallback(WiFiEvent_t event);

class EthManager
{
  public:
  /* Constructor */
  EthManager();

  /* Begin function */
  void begin(void);

  /* Loop function */
  void loop(void);

  /* Get connection status */
  bool connected(void);

  /* Get local name string */
  String name(void);

  protected:
  friend void EthManagerCallback(WiFiEvent_t event);
  /* Cached name string */
  String m_Name;

  /* Connected status */
  bool m_Connected;
};


/* Global Instance */
extern class EthManager Network;

#endif
