// #include <WiFi.h>
// #include "time.h"
// #include <NTPClient.h>
// #include <WiFiUdp.h>

// const char* ssid = "TP-Link_CA30";
// const char* password = "29451760";

// // const char* ntpServer = "pool.ntp.org";
// // const long gmtOffset_sec = 25200; // GMT +7
// // const int daylightOffset_sec = 0; // ไม่มี daylight saving time ในประเทศไทย

// //ตั้งค่า NTP
// WiFiUDP ntpUDP;
// NTPClient timeClient(ntpUDP, "pool.ntp.org", 25200, 60000);

// //set up ip address
// IPAddress ip(172, 16, 46, 17);  
// IPAddress gateway(172,16,46,254);
// IPAddress subnet(255, 255, 255, 0);

// // String getFormattedTime() {
// //   struct tm timeinfo;
// //   timeval tv;
// //   if (!getLocalTime(&timeinfo)) {
// //     Serial.println("Failed to obtain time");
// //     return "";
// //   }
// //   gettimeofday(&tv, nullptr);

// //   char buffer[30];
// //   snprintf(buffer, sizeof(buffer), "%04d-%02d-%02dT%02d:%02d:%02d.%03ld+0700",
// //            timeinfo.tm_year + 1900,
// //            timeinfo.tm_mon + 1,
// //            timeinfo.tm_mday,
// //            timeinfo.tm_hour,
// //            timeinfo.tm_min,
// //            timeinfo.tm_sec,
// //            tv.tv_usec / 1000);

// //   return String(buffer);
// // }

// // long getTimestamp() {
// //   struct timeval tv;
// //   gettimeofday(&tv, nullptr);
// //   return tv.tv_sec ;
// // }
// String getFormattedTime() {
//   timeClient.update();
//   unsigned long rawTime = timeClient.getEpochTime();
//   struct tm *ptm = gmtime((time_t *)&rawTime);

//   // Get milliseconds
//   struct timeval tv;
//   gettimeofday(&tv, NULL);
//   int millisec = tv.tv_usec / 1000;

//   char buffer[30];
//   snprintf(buffer, sizeof(buffer), "%04d-%02d-%02dT%02d:%02d:%02d.%03d+0000",
//            ptm->tm_year + 1900,
//            ptm->tm_mon + 1,
//            ptm->tm_mday,
//            ptm->tm_hour,
//            ptm->tm_min,
//            ptm->tm_sec,
//            millisec);
//   return String(buffer);
// }

// long getTimestamp() {
//   struct timeval tv;
//   gettimeofday(&tv, NULL);
//   return tv.tv_sec * 1000L + tv.tv_usec / 1000;
// }

// void setup() {
//   Serial.begin(115200);

//   // Connect to Wi-Fi
//   Serial.print("Connecting to ");
//   Serial.println(ssid);
//   WiFi.begin(ssid, password);
//   WiFi.config(ip, gateway, subnet);
//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500);
//     Serial.print(".");
//   }
//   Serial.println("");
//   Serial.println("WiFi connected.");
  
//   // Init and get the time
//   // configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
//   timeClient.begin();
// }

// void loop() {
//   delay(1000);
//   Serial.print("date: ");
//   Serial.println(getFormattedTime());
//   Serial.print("timestamp: ");
//   Serial.println(getTimestamp());
// }

#include <WiFi.h>
#include "time.h"

const char* ssid       = "YOUR_SSID";
const char* password   = "YOUR_PASS";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;

void printLocalTime()
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

void setup()
{
  Serial.begin(115200);
  
  //connect to WiFi
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println(" CONNECTED");
  
  //init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();

  //disconnect WiFi as it's no longer needed
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
}

void loop()
{
  delay(1000);
  printLocalTime();
}

