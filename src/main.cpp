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

ConfigManager *pConfigManager;

ResetDetector *pResetDetector;

WiFiConfig gWiFiConfig;
WiFiManager *pWiFiManager;

AmbientSensorConfig gAmbientSensorConfig;
AmbientSensor *pAmbientSensor;

MqttConfig gMqttConfig;
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
  mqttConfig.baseTopic = String(configParameters["mqtt.baseTopic"]);
  mqttConfig.username = String(configParameters["mqtt.username"]);
  mqttConfig.password = String(configParameters["mqtt.password"]);
  pConfigManager->write<MqttConfig>(mqttConfig);

  pResetDetector->clear();
  ESP.restart();
}

void handleMeasurementCallback(Measurement<float> temperature, Measurement<float> humidity)
{
  // return early if WiFi connection is not established and cannot be established
  if (!pWiFiManager->isConnected() && !pWiFiManager->reconnect(3))
  {
    return;
  }

  // return early if MQTT connection is not established and cannot be established
  if (!pMqttClient->isConnected() && !pMqttClient->connect(3))
  {
    return;
  }

  pMqttClient->publish(temperature.quantity, temperature.serialize());
  pMqttClient->publish(humidity.quantity, humidity.serialize());

  pMqttClient->disconnect();
  pWiFiManager->disconnect();
}

void setup()
{
  Helpers::init();

  Helpers::ledFlasher.start(250);

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
  pResetDetector = new ResetDetector(*pConfigManager);

  if (pResetDetector->shouldReset())
  {
    pResetDetector->clear();
    pConfigManager->reset(ConfigManager::ResetMode::HardReset);
    ESP.restart();
  }

  if (!Helpers::hasWokenUpFromDeepSleep())
  {
    pResetDetector->go(15 * 1000);
  }

  if (pConfigManager->exists(gWiFiConfig))
  {
    pConfigManager->read(gWiFiConfig);
  }

  pWiFiManager = new WiFiManager(Helpers::getChipId(), gWiFiConfig);

  bool result = pWiFiManager->tryAutoConnect();

  if (!result)
  {
    pWiFiManager->setConfigPortalCallback(&handleConfigPortalCallback);

    // sensor
    pWiFiManager->addConfigParameter("sensor.type", "Sensor Type", "22", 2, "maxlength='2' spellcheck='false' required");
    pWiFiManager->addConfigParameter("sensor.temperatureUnit", "Temperature Unit", "1", 1, "maxlength='1' spellcheck='false' required");
    pWiFiManager->addConfigParameter("sensor.measurementIntervalInSeconds", "Measurement Interval (s)", "30", 5, "type='number' min='15' max='1800' step='1' required");

    // MQTT
    pWiFiManager->addConfigParameter("mqtt.host", "MQTT Host", "", 128, "maxlength='128' spellcheck='false' required");
    pWiFiManager->addConfigParameter("mqtt.port", "MQTT Port", "1883", 5, "type='number' min='0' max='65535' step='1' required");
    pWiFiManager->addConfigParameter("mqtt.clientId", "MQTT Client ID", "", 64, "maxlength='64' spellcheck='false' required");
    pWiFiManager->addConfigParameter("mqtt.baseTopic", "MQTT Base Topic", "", 128, "maxlength='128' spellcheck='false' required");
    pWiFiManager->addConfigParameter("mqtt.username", "MQTT Username", "", 32, "maxlength='32' spellcheck='false'");
    pWiFiManager->addConfigParameter("mqtt.password", "MQTT Password", "", 16, "type='password' maxlength='16'");

    result = pWiFiManager->startConfigPortal(Helpers::getChipId().c_str());
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
}

void loop()
{
  Helpers::ledFlasher.start(50);

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