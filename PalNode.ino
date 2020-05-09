/* Main source code file for PalNode
 * This acts as a configuration file,
 * The actual code is in cpp files.
 */

#include "EthManager.h"
#include "TaskCCS811.h"
#include "Wire.h"

void setup()
{
  Serial.begin(115200);
  Wire.begin(13,16);
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
  Network.begin();
  CCS.begin(5.0f);
}

void loop()
{
  Network.loop();
}
