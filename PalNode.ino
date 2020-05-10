/* Main source code file for PalNode
 * This acts as a configuration file,
 * The actual code is in cpp files.
 * The following libraries are required to be installed via the library manager:
 * 
 */

#include "EthManager.h"
#include "TaskCCS811.h"
#include "TaskBME680.h"
#include "TaskPMS.h"
#include "TaskBLE.h"
#include "Wire.h"

/* Function to enumerate I2C devices */
void ListI2C()
{
  for(int address = 1; address < 127; address++ ) {
    Wire.beginTransmission(address);
    int error = Wire.endTransmission();
    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address<16) {
        Serial.print("0");
      }
      Serial.println(address,HEX);
    }
    else if (error==4) {
      Serial.print("Unknow error at address 0x");
      if (address<16) {
        Serial.print("0");
      }
      Serial.println(address,HEX);
    }    
  }  
}

void setup()
{
  /* Start Serial */
  Serial.begin(115200);

  /* Start Wire using ports for ESP32-POE board */
  Wire.begin(13,16);

  /* Poll I2C Bus */
  ListI2C();

  /* Start networking (and MQTT) */
  Network.begin();

#ifdef SENSOR_BME680
  /* Start BME680 */
  BME.begin(5.0f);
#endif

#ifdef SENSOR_CCS811
  /* Start CCS811 */
  CCS.begin(5.0f);
#endif

#ifdef SENSOR_PMS
  /* Start PMS @ 2 minute interval */
  PMS.begin(2*60.0f);
#endif

#ifdef SENSOR_BLE
  /* Start BLE scanner @ 5sec scan on 20s interval, passive */
  BLE.begin(20,5,false);
#endif
}

void loop()
{
  Network.loop();
}
