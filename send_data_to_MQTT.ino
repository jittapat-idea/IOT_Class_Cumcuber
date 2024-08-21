// send ค่าผ่าน MQTT 
#include <Wire.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <PubSubClient.h>

// read ค่าจาก senser
#include <Adafruit_BMP280.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_HTS221.h>
#include <SensirionI2cSht4x.h>

// ตั้งค่า time
#include <time.h>
#include <ESPNtpClient.h>
// #include <NTPClient.h>
// #include <WiFiUdp.h>

#define NTP_TIMEOUT 5000

//ตั้งค่าตัวแปร senser
Adafruit_BMP280 bmp;//วัดความดันอากาศแบบสัมบูรณ์ (300 ~ 1100 hPa)
Adafruit_HTS221 hts;//วัดอุณหภูมิ (-40 ~ 120 °C) และความชื้น (0 ~ 100 %RH)
Adafruit_MPU6050 mpu;//วัดตรวจจับการเคลื่อนไหว (ความเร่งแบบ 3 แกน & ไจโรสโคปแบบ 3 แกน)
SensirionI2cSht4x sensor;

const PROGMEM char* ntpServer = "172.16.46.11"; //ใช้ ntp server จาก server ของเรา 

int sensorPin = 5; // select the input pin for the potentiometer
int sensorValue = 0; // variable to store the value coming from the sensor

//WIFI
const char* ssid = "TP-Link_CA30";
const char* password = "29451760";

//set Mqtt server
const char* mqtt_server = "172.16.46.11";
const int mqttPort = 1883;
const char* mqttUser = "liam-sensor-3";
const char* mqttPassword = "1q2w3e4r";

//set up ip address
IPAddress ip(172, 16, 46, 17);  
IPAddress gateway(172,16,46,11);
IPAddress subnet(255, 255, 255, 0);


WiFiClient espClient;
PubSubClient client(espClient);

void setupHardware(){
  //Wire.begin(41,40,100000);
  if (bmp.begin(0x76)){
    Serial.println("BMP280 sensor ready");
  }
  
  if (hts.begin_I2C()){
    Serial.println("HTS221 sensor ready");
    //while(1);
  }
  if (mpu.begin()) { // prepare MPU6050 sensor
   Serial.println("MPU6050 sensor ready!");
  }
  pinMode(2,OUTPUT);
  digitalWrite(2,HIGH);
}
void setupWifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  WiFi.config(ip, gateway, subnet);


  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
void reconnect(){
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
 
    if (client.connect("idea_sensor3_is_here",mqttUser,mqttPassword)) {
 
      Serial.println("connected");
 
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();
}

unsigned long Get_Epoch_time(){
  time_t now;
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    return 0;
  }
  time(&now);
  return now;
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Wire.begin(41,40,100000);
  sensor.begin(Wire,SHT40_I2C_ADDR_44);
  
  setupHardware();
  sensor.softReset();
  delay(10);

  mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.println("Starting");

  setupWifi();
  client.setServer(mqtt_server, mqttPort);
  client.setCallback(callback);

  
  // Init and get the time
  // configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  NTP.setTimeZone (TZ_Asia_Bangkok);
  NTP.setInterval(600);
  NTP.setNTPTimeout(NTP_TIMEOUT);
  NTP.begin (ntpServer);

}

void loop() {
  // put your main code here, to run repeatedly:
  if (!client.connected()) {
      reconnect();
  }
  client.loop();

//========================= Get Data from Sensers ========================
  sensors_event_t temp, humidity, a, g, temp2;
  mpu.getEvent(&a, &g, &temp2);

  //get HTS221(Sensirion)(วันอุณหภูมิ,ความชื่น)
  float aHumidity = 0.0;
  float aTemperature = 0.0;
  sensor.measureHighPrecision(aTemperature, aHumidity);

  //get bmp(วัดความดันอากาศ)
  float bmpTemperature = bmp.readTemperature();
  float bmpPressure = bmp.readPressure();

  //get mpu(วัดตรวจจับการเคลื่อนไหว)
  float X_value = a.acceleration.x;
  float Y_value = a.acceleration.y;
  float Z_value = a.acceleration.z;

  //get LDR (วัดแสง)
  sensorValue = map(analogRead(sensorPin), 1900, 5000, 0, 100);
//======================== สร้าง JSON ====================================
  //StaticJsonDocument<200> doc;
  // doc["mpu"]["x"] = X_value;
  // doc["mpu"]["y"] = Y_value;
  // doc["mpu"]["z"] = Z_value;
  // doc["bmp280"]["temperature"] = bmpTemperature;
  // doc["bmp280"]["pressure"] = bmpPressure;
  // doc["hts221(sht4x)"]["temperature"] = aTemperature;
  // doc["hts221(sht4x)"]["humidity"] = aHumidity;
  // doc["ldr"] = sensorValue;
  unsigned long epochTime = Get_Epoch_time();

  StaticJsonDocument<200> doc;
  doc["id"] = "43245253";
  doc["name"] = "liam-sensor-3";
  doc["place_id"] = "42343243";
  doc["date"] = NTP.getTimeDateString(time(NULL),"%Y-%m-%dT%H:%M:%S");
  doc["timestamp"] = epochTime;

  JsonObject payload = doc.createNestedObject("payload");
  payload["temperature"] = aTemperature;
  payload["humidity"] = aHumidity;
  payload["pressure"] = bmpPressure;
  payload["luminosity"] = sensorValue;

  char buffer[256];
  serializeJson(doc, buffer);

  // ส่ง JSON ไปยัง topic ที่กำหนด
  client.publish("iot-frames", buffer);
 
  Serial.print("date: ");
  Serial.print(NTP.getTimeDateString(time(NULL),"%Y-%m-%dT%H:%M:%S"));

  Serial.print("time stamp: ");
  Serial.print(epochTime);

  Serial.print("HTS221(Sensirion) sensor Temp: ");
  Serial.print(aTemperature);
  Serial.print(" degrees C, Humidity: ");
  Serial.print(aHumidity);
  Serial.println("% rH");

  Serial.print("BMP280 sensor Temp: ");
  Serial.print(bmp.readTemperature());
  Serial.print("*C, Pressure: ");
  Serial.print(bmp.readPressure());
  Serial.println("Pa");

  Serial.print("LDR sensor: ");
  Serial.println(sensorValue, DEC);


  delay(2000);

}