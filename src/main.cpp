#ifdef ESP32
#define PIN 4
#else
#define PIN D1
#endif

#include <Arduino.h>
#include <utilities/Helpers.h>
#include <config/ConfigManager.h>
#include <config/models/WiFiConfig.h>
#include <config/models/AmbientSensorConfig.h>
#include <config/models/MqttConfig.h>
#include <utilities/ResetDetector.h>
#include <communication/WiFiManager.h>
#include <communication/MqttClient.h>
#include <sensors/AmbientSensor.h>

ResetDetector *pResetDetector;
ConfigManager *pConfigManager;
WiFiManager *pWiFiManager;

AmbientSensorConfig gAmbientSensorConfig;
MqttConfig gMqttConfig;

AmbientSensor *pAmbientSensor;
MqttClient *pMqttClient;

void handleConfigPortalCallback(bool success, WiFiConfig &wiFiConfig, std::map<const char *, const char *> &configParameters)
{
  Serial.print(F("Config portal returned status: "));
  Serial.println(success ? F("SUCCESS") : F("NO SUCCESS"));

  pConfigManager->write<WiFiConfig>(wiFiConfig);

  AmbientSensorConfig ambientSensorConfig(configParameters["sensor.type"], configParameters["sensor.temperatureUnit"], atoi(configParameters["sensor.measurementIntervalInSeconds"]));
  pConfigManager->write<AmbientSensorConfig>(ambientSensorConfig);

  MqttConfig mqttConfig;
  mqttConfig.host = String(configParameters["mqtt.host"]);
  mqttConfig.port = atoi(configParameters["mqtt.port"]);
  mqttConfig.clientId = String(configParameters["mqtt.clientId"]);
  mqttConfig.topic = String(configParameters["mqtt.topic"]);
  mqttConfig.username = String(configParameters["mqtt.username"]);
  mqttConfig.password = String(configParameters["mqtt.password"]);
  pConfigManager->write<MqttConfig>(mqttConfig);

  // workaround because WiFi manager sometimes can't properly connect to WiFi after config portal
  if (!pWiFiManager->isConnected())
  {
    pResetDetector->disable();
    ESP.restart();
  }
}

void handleMeasurementCallback(Measurement<float> temperature, Measurement<float> humidity)
{
  // return early if WiFi connection is not established and cannot be established
  if (!pWiFiManager->isConnected() && !pWiFiManager->tryAutoConnect(3))
  {
    return;
  }

  // return early if MQTT connection is not established and cannot be established
  if (!pMqttClient->isConnected() && !pMqttClient->connect(3))
  {
    return;
  }

  StaticJsonDocument<192> doc;
  JsonObject temperatureObj = doc.createNestedObject();
  temperature.populateJsonObject(temperatureObj);
  JsonObject humidityObj = doc.createNestedObject();
  humidity.populateJsonObject(humidityObj);

  String message;
  serializeJson(doc, message);

  pMqttClient->publish(message.c_str());

  pMqttClient->disconnect();
  pWiFiManager->disconnect();
}

void setup()
{
  Serial.begin(115200);

  while (!Serial)
  {
    delay(250);
  }

  // usually, object instances created with keyword 'new' need to be deleted manually
  // however, since this object instance is need for the entire lifetime of the program, it is useless to declare memory reclaiming for it
  pConfigManager = new ConfigManager();

  // usually, object instances created with keyword 'new' need to be deleted manually
  // however, since this object instance is need for the entire lifetime of the program, it is useless to declare memory reclaiming for it
  pResetDetector = new ResetDetector(pConfigManager, 15);

  if (Helpers::hasStartedFromDeepSleep())
  {
    pResetDetector->disable();
  }
  else if (pResetDetector->shouldReset())
  {
    pConfigManager->reset(pConfigManager->HardReset);
    ESP.restart();
  }

  WiFiConfig wiFiConfig;
  if (pConfigManager->exists(wiFiConfig))
  {
    pConfigManager->read(wiFiConfig);
  }

  WiFiManager wiFiManager(Helpers::getChipId(), wiFiConfig);
  pWiFiManager = &wiFiManager;

  bool result = wiFiManager.tryAutoConnect();

  if (!result)
  {
    wiFiManager.setConfigPortalCallback(&handleConfigPortalCallback);

    // sensor
    wiFiManager.addConfigParameter("sensor.type", "Sensor Type", "22", 2, "maxlength='2' spellcheck='false' required");
    wiFiManager.addConfigParameter("sensor.temperatureUnit", "Temperature Unit", "1", 1, "maxlength='1' spellcheck='false' required");
    wiFiManager.addConfigParameter("sensor.measurementIntervalInSeconds", "Measurement Interval (s)", "30", 5, "type='number' min='15' max='1800' step='1' required");

    // MQTT
    wiFiManager.addConfigParameter("mqtt.host", "MQTT Host", "", 128, "maxlength='128' spellcheck='false' required");
    wiFiManager.addConfigParameter("mqtt.port", "MQTT Port", "1883", 5, "type='number' min='0' max='65535' step='1' required");
    wiFiManager.addConfigParameter("mqtt.clientId", "MQTT Client ID", "", 64, "maxlength='64' spellcheck='false' required");
    wiFiManager.addConfigParameter("mqtt.topic", "MQTT Topic", "", 128, "maxlength='128' spellcheck='false' required");
    wiFiManager.addConfigParameter("mqtt.username", "MQTT Username", "", 32, "maxlength='32' spellcheck='false'");
    wiFiManager.addConfigParameter("mqtt.password", "MQTT Password", "", 16, "type='password' maxlength='16'");

    result = wiFiManager.startConfigPortal(Helpers::getChipId().c_str());
  }

  pConfigManager->read(gAmbientSensorConfig);
  // usually, object instances created with keyword 'new' need to be deleted manually
  // however, since this object instance is need for the entire lifetime of the program, it is useless to declare memory reclaiming for it
  pAmbientSensor = new AmbientSensor(PIN, gAmbientSensorConfig);
  pAmbientSensor->setMeasurementCallback(&handleMeasurementCallback);

  pConfigManager->read(gMqttConfig);
  // usually, object instances created with keyword 'new' need to be deleted manually
  // however, since this object instance is need for the entire lifetime of the program, it is useless to declare memory reclaiming for it
  pMqttClient = new MqttClient(gMqttConfig);
  pMqttClient->connect();
}

void loop()
{
  pResetDetector->process();
  pAmbientSensor->measure();

  // return early to allow for disabling reset detection
  if (pResetDetector->isEnabled())
  {
    delay(250);
    return;
  }

  Helpers::setMillisOffset(gAmbientSensorConfig.measurementIntervalInSeconds * 1000);
  Serial.println(F("Going into deep sleep..."));
  // subtract 10 seconds from millis offset because this is rougly what the overhead to establish a communication takes until we can measure
  ESP.deepSleep((gAmbientSensorConfig.measurementIntervalInSeconds - 10) * 1000000);
  yield();
  Serial.println(F("Error: Deep sleep failed."));
}