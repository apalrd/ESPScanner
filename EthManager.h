/* Ethernet Manager
 * This file deals with the global Ethernet and MQTT state
 */
#ifndef __ETH_MANAGER_H
#define __ETH_MANAGER_H
#include <ETH.h>

/* Callback function (Not a class member */
void EthManagerCallback(WiFiEvent_t event);

class EthManager
{
  public:
  /* Constructor */
  EthManager();

  /* Begin function */
  void begin(void);

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
