/* Ethernet Manager
 * This file deals with the global Ethernet and MQTT state
 */

#include "EthManager.h"
#include "TaskBME680.h"
#include "Wire.h"
#include <bsec.h>
#include <Preferences.h>


/* Global BSEC object */
static Bsec LocalBsec;

/* Global BME object */
class TaskBME680 BME;

/* Global Preferences object */
static Preferences Prefs;

/* Handles for local tasks */
static TaskHandle_t xTaskPoll = NULL, xTaskBgnd = NULL;


/* BSEC global data area */
static bsec_virtual_sensor_t SensorList[] = {
  BSEC_OUTPUT_RAW_TEMPERATURE,
  BSEC_OUTPUT_RAW_PRESSURE,
  BSEC_OUTPUT_RAW_HUMIDITY,
  BSEC_OUTPUT_RAW_GAS,
  BSEC_OUTPUT_IAQ,
  BSEC_OUTPUT_STATIC_IAQ,
  BSEC_OUTPUT_CO2_EQUIVALENT,
  BSEC_OUTPUT_BREATH_VOC_EQUIVALENT,
  BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE,
  BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY,
};

static uint8_t BsecState[BSEC_MAX_STATE_BLOB_SIZE] = {0};

static const uint8_t BsecConfig[] = {
/* Contents of generic_33v_3s_4d */
4,7,4,1,61,0,0,0,0,0,0,0,174,1,0,0,48,0,1,0,0,192,168,71,64,49,119,76,0,0,225,68,137,65,0,63,205,204,204,62,0,0,
64,63,205,204,204,62,0,0,0,0,216,85,0,100,0,0,0,0,0,0,0,0,28,0,2,0,0,244,1,225,0,25,0,0,128,64,0,0,32,65,144,1,
0,0,112,65,0,0,0,63,16,0,3,0,10,215,163,60,10,215,35,59,10,215,35,59,9,0,5,0,0,0,0,0,1,88,0,9,0,229,208,34,62,0,
0,0,0,0,0,0,0,218,27,156,62,225,11,67,64,0,0,160,64,0,0,0,0,0,0,0,0,94,75,72,189,93,254,159,64,66,62,160,191,0,
0,0,0,0,0,0,0,33,31,180,190,138,176,97,64,65,241,99,190,0,0,0,0,0,0,0,0,167,121,71,61,165,189,41,192,184,30,189,
64,12,0,10,0,0,0,0,0,0,0,0,0,229,0,254,0,2,1,5,48,117,100,0,44,1,112,23,151,7,132,3,197,0,92,4,144,1,64,1,64,1,
144,1,48,117,48,117,48,117,48,117,100,0,100,0,100,0,48,117,48,117,48,117,100,0,100,0,48,117,48,117,100,0,100,0,
100,0,100,0,48,117,48,117,48,117,100,0,100,0,100,0,48,117,48,117,100,0,100,0,44,1,44,1,44,1,44,1,44,1,44,1,44,1,
44,1,44,1,44,1,44,1,44,1,44,1,44,1,8,7,8,7,8,7,8,7,8,7,8,7,8,7,8,7,8,7,8,7,8,7,8,7,8,7,8,7,112,23,112,23,112,23,
112,23,112,23,112,23,112,23,112,23,112,23,112,23,112,23,112,23,112,23,112,23,255,255,255,255,255,255,255,255,220,
5,220,5,220,5,255,255,255,255,255,255,220,5,220,5,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,44,1,0,0,0,0,138,80,0,0
};

/* Task to poll sensor */
void TaskBME680_Poll(void *pvParameters)
{
  (void) pvParameters;
  Serial.println("BME680: Task Starting");
  static char Topic[128];
  static String output;
  static int32_t LastSaveTime;
  static bool HasSaved = false;
  bool SaveNow = false;

  /* Loop forever */
  do
  {
    /* Poll the sensor */
    if(LocalBsec.run())
    {
      /* New sensor data */
      BME.m_Temp = LocalBsec.temperature;
      BME.m_Humid = LocalBsec.humidity;

      /* Format MQTT response */
      sprintf(Topic,"raw/esp-%s/bme680",Network.name().c_str());
      output = "{ ";
      output += "\"Prs\": "+String(LocalBsec.pressure/1000.0f);
      //output += "\"RawTemp\": "+String(LocalBsec.rawTemperature);
      //output += ", \"RawHumid\": "+String(LocalBsec.rawHumidity);
      output += ", \"RawGas\": "+String(LocalBsec.gasResistance);
      output += ", \"Temp\": "+String(LocalBsec.temperature);
      output += ", \"Humid\": "+String(LocalBsec.humidity);
      output += ", \"IAQ\": "+String(LocalBsec.iaq);
      output += ", \"IAQAcc\": "+String(LocalBsec.iaqAccuracy);
      output += ", \"CO2\": "+String(LocalBsec.co2Equivalent);
      output += ", \"VOC\": "+String(LocalBsec.breathVocEquivalent);
      output += "}";
      Serial.print("BME680: ");
      Serial.println(output);
      if(MQTT.connected() && (LocalBsec.status == BSEC_OK) && (LocalBsec.bme680Status == BME680_OK))
      {
        MQTT.publish_P(Topic,output.c_str(),true);
      }

      /* Save because Accuracy changed and we haven't saved yet */
      if((!HasSaved && LocalBsec.iaqAccuracy >= 3) ||
        /* Save because it's been 1 hour since we saved */
        ((millis() - LastSaveTime) > (1000*60*60)))
      {
        /* Send a notification to the background task */
        xTaskNotifyGive(xTaskBgnd);
        LastSaveTime = millis();
        HasSaved = true;
      }
    }
    else
    {
      BME.status();
    }

    /* Delay a little bit */
    vTaskDelay(100);
  } while(1);

}

/* Task to update state preferences */
void TaskBME680_Bgnd(void *pvParameters)
{
  (void) pvParameters;
  Serial.println("BME680: Background Starting");

  /* Loop forever */
  do
  {
    /* Wait for a notification */
    ulTaskNotifyTake(pdTRUE,portMAX_DELAY);

    /* Write NVM */
    BME.stateSave();
    
  } while(1);

}
  

/* Constructors - set read interval to 5s by default */
TaskBME680::TaskBME680()
{
  m_readInterval = 5.0f;
}

/* Begin function */
void TaskBME680::begin(float readInterval)
{
  m_readInterval = readInterval;
  begin();
}
void TaskBME680::begin(void)
{
  /* Initialize reasonable temperatures and humidities */
  m_Temp = 25.0f;
  m_Humid = 20.0f;

  /* Initialize preferences for BME680 */
  Prefs.begin("BME680");
  
  /* Start sensor and check for errors */
  LocalBsec.begin(0x77, Wire);
  String output = "BME680: BSEC library version " + String(LocalBsec.version.major) + "." + String(LocalBsec.version.minor) + "." + String(LocalBsec.version.major_bugfix) + "." + String(LocalBsec.version.minor_bugfix);
  Serial.println(output);
  status();

  /* Setup configuration */
  Serial.println("BME680: Configuring IAQ");
  LocalBsec.setConfig(BsecConfig);
  status();

  /* Setup BSEC */
  Serial.println("BME680: Updating Subscriptions");
  LocalBsec.updateSubscription(SensorList,sizeof(SensorList)/sizeof(bsec_virtual_sensor_t),BSEC_SAMPLE_RATE_LP);
  status();
  
  /* Start sensor */
  if(LocalBsec.status == BSEC_OK && LocalBsec.bme680Status == BME680_OK)
  {
    /* Successfully started sensor, setup polling task */
    Serial.println("BME680: Sensor Initialized");
    xTaskCreatePinnedToCore(
      TaskBME680_Poll,
      "TaskBME680",
      8192,
      NULL,
      2,
      &xTaskPoll,
      ARDUINO_RUNNING_CORE);
    /* Start background task to update preferences */
    xTaskCreatePinnedToCore(
      TaskBME680_Bgnd,
      "TaskBME680Bgnd",
      8192,
      NULL,
      0,
      &xTaskBgnd,
      ARDUINO_RUNNING_CORE);
  }
  else
  {
    /* Failed to start :( */
    Serial.println("BME680: Failed to start sensor!");
  }
}

/* Get BSEC / BME680 status */
void TaskBME680::status(void)
{
  String output;
  if(LocalBsec.status != 0)
  {
    output = "BME680: BSEC Status: "+String(LocalBsec.status);
    Serial.println(output);
  }
  if(LocalBsec.bme680Status != 0)
  {
    output = "BME680: Sensor Status: "+String(LocalBsec.bme680Status);
    Serial.println(output); 
  }
}

/* Load BSEC state from nvm */
void TaskBME680::stateLoad(void)
{
  /* Get Prefs size and see if it matches expected */
  uint32_t PrefSize = Prefs.getBytesLength("BSEC");
  if(PrefSize != BSEC_MAX_STATE_BLOB_SIZE)
  {
    Serial.println("BME680: ERROR Prefs are invalid!");
  }
  else
  {
    /* Read BsecState and update BSEC */
    Prefs.getBytes("BSEC",BsecState,BSEC_MAX_STATE_BLOB_SIZE);
    LocalBsec.setState(BsecState);
    status();
  }
}

/* Save BSEC state to NVM */
void TaskBME680::stateSave(void)
{
  /* Get state from BSEC */
  LocalBsec.getState(BsecState);
  status();
  Serial.println("BME680: Writing state preferences...");
  Prefs.putBytes("BSEC",BsecState,BSEC_MAX_STATE_BLOB_SIZE);
}

float TaskBME680::getTemp(void) {return m_Temp;}
float TaskBME680::getHumid(void) {return m_Humid;}
