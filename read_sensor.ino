#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_HTS221.h>
#include <SensirionI2cSht4x.h>

#include <Adafruit_NeoPixel.h> 

#define NUM_PIXELS  (8)            // The number of pixels
#define WS2812_PIN  GPIO_NUM_18 // The ESP32 pin for DATA output

// Create an object from the 'Adafruit_NeoPixel' class 
Adafruit_NeoPixel pixels(NUM_PIXELS, WS2812_PIN, NEO_RGB + NEO_KHZ800);

Adafruit_BMP280 bmp;//วัดความดันอากาศแบบสัมบูรณ์ (300 ~ 1100 hPa)
Adafruit_HTS221 hts;//วัดอุณหภูมิ (-40 ~ 120 °C) และความชื้น (0 ~ 100 %RH)
Adafruit_MPU6050 mpu;//วัดตรวจจับการเคลื่อนไหว (ความเร่งแบบ 3 แกน & ไจโรสโคปแบบ 3 แกน)
SensirionI2cSht4x sensor;

//ESP32-S2 ทำงานเป็น I2C master แล้วสั่งอ่าน/เขียนค่ารีจิสเตอร์ของแต่ละเซ็นเซอร์
//ด้วยการระบุค่าแอดเดรส โดยค่าแอดเดรสของ 
//HTS221 คือ 0x5F, BMP280 คือ 0x76 และ MPU-6050 คือ 0x68 

int sensorPin = 5; // select the input pin for the potentiometer
int sensorValue = 0; // variable to store the value coming from the sensor

// Colors for different states
const uint32_t LED_COLOR_STARTING = 0x00FF00; // Green
const uint32_t LED_COLOR_WIFI_CONNECTED = 0x0000FF; // Blue
const uint32_t LED_COLOR_MQTT_CONNECTED = 0xFFFF00; // Yellow
const uint32_t LED_COLOR_SENDING_DATA = 0xFF0000; // Red

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

void setLED(uint32_t color) {
  for (uint16_t i = 0; i < NUM_PIXELS; i++) {
    pixels.setPixelColor(i, color);
  }
  pixels.show();
}
void blinkLED(uint32_t color, int blinkTimes, int delay_us) {
  for (int i = 0; i < blinkTimes; i++) {
    setLED(color);           // Turn on the LED with the specified color
    delayMicroseconds(delay_us); // Wait for the specified microseconds
    setLED(0x000000);        // Turn off the LED
    delayMicroseconds(delay_us); // Wait again
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Wire.begin(41,40,100000);
  sensor.begin(Wire,SHT40_I2C_ADDR_44);

  pixels.begin(); // Initialize the NeoPixel WS2812 strip.
  pixels.setBrightness(255); // Set the brightness to 255.

  setLED(LED_COLOR_STARTING);
  
  setupHardware();
  sensor.softReset();
  delay(10);

  mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.println("Starting");

  // Simulate WiFi connection (replace with actual WiFi connection code)
  delay(2000); // Simulate delay for WiFi connection
  setLED(LED_COLOR_WIFI_CONNECTED); // Indicate WiFi connection
  
  // Simulate MQTT connection (replace with actual MQTT connection code)
  delay(2000); // Simulate delay for MQTT connection
  setLED(LED_COLOR_MQTT_CONNECTED); // Indicate MQTT connection
}

void loop() {

   // Simulate data sending process
  blinkLED(LED_COLOR_SENDING_DATA,10,50000);//Blink red 10 times with 50ms on/off

  //put your main code here, to run repeatedly:
  sensors_event_t temp, humidity, a, g, temp2;
  //hts.getEvent(&humidity, &temp);
  mpu.getEvent(&a, &g, &temp2);


  float aHumidity = 0.0;
  float aTemperature = 0.0;
  sensor.measureHighPrecision(aTemperature,aHumidity);

  Serial.print('\n');
  Serial.print("MPU6050 sensor X: ");
  Serial.print(a.acceleration.x);
  Serial.print(", Y: ");
  Serial.print(a.acceleration.y);
  Serial.print(", Z: ");
  Serial.print(a.acceleration.z);
  Serial.println(" m/s^2");

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

  sensorValue = map(analogRead(sensorPin),1600,6000,0,65);
  //sensorValue = analogRead(sensorPin);
  Serial.print("LDR sensor: ");
  Serial.println(sensorValue, DEC);

  delay(2000);
}
