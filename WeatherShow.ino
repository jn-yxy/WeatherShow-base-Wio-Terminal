#include <Arduino.h>
#include "SPI.h"
#include <Seeed_FS.h>
#include <rpcWiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <TFT_eSPI.h>
#include "SD/Seeed_SD.h"

#define TFT_GREY 0x5AEB
TFT_eSPI tft;

// const char *ssid = "OP";
// const char *password = "261020!)@)";
const char *ssid = "540";     //填入SSID
const char *password = "540nicai"; //填入Wifi密码
String Citycode = "XXXXXXXXXXXXXX";      //查询所在区域的城市编码填入
                                         //https://github.com/baichengzhou/weather.api/blob/master/src/main/resources/citycode-2019-08-23.json
int forecastCon = 0;
String payload;

uint16_t modeUI = 0;

void getData();
void drawUI();
void decodeJSON();
void setup()
{
  tft.init();
  tft.setRotation(3); //旋转屏幕0-3
  tft.fillScreen(TFT_BLACK);
  tft.drawString("RTL8720 Firmware Version", (320 - tft.textWidth("RTL8720 Firmware Version")) / 2, 100);
  tft.drawString(rpc_system_version(), (320 - tft.textWidth(rpc_system_version())) / 2, 116);
  tft.drawString("Initializing", (320 - tft.textWidth("Initializing")) / 2, 132);
  Serial.begin(115200);
  for (uint8_t t = 2; t > 0; t--)
  {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }
  Serial.printf("RTL8720 Firmware Version: %s\n", rpc_system_version());
  tft.drawString("Connecting to Wifi", (320 - tft.textWidth("Connecting to Wifi")) / 2, 116);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    // wait 1 second for re-trying
    delay(1000);
  }
  Serial.print("WiFi Connected\n");
  tft.drawString("WiFi Connected", (320 - tft.textWidth("WiFi Connected")) / 2, 116);

  // 中文

  while (!SD.begin(SDCARD_SS_PIN, SDCARD_SPI))
  {
    Serial.println("SD card error!\n");
    while (1)
      ;
  }

  delay(1000);

  pinMode(WIO_KEY_A, INPUT);
  pinMode(WIO_KEY_B, INPUT);
  //payload = jsonStr;
  getData();
  Serial.println(payload);
  tft.drawString("Get Data OK", (320 - tft.textWidth("Get Data OK")) / 2, 216);
  drawUI();
  decodeJSON();
}

void loop()
{
  if (0 == digitalRead(WIO_KEY_A))
  {
    while (0 == digitalRead(WIO_KEY_A))
    {
    }
    Serial.println("Begin Get Data");
    tft.drawString("Begin Get Data", (320 - tft.textWidth("Begin Get Data")) / 2, 216);
    //payload = jsonStr;
    getData();
    Serial.println(payload);
    tft.drawString("Get Data OK", (320 - tft.textWidth("Get Data OK")) / 2, 216);
    drawUI();
    decodeJSON();
  }
  if (0 == digitalRead(WIO_KEY_B))
  {
    while (0 == digitalRead(WIO_KEY_B))
    {
    }
    modeUI = (modeUI + 1) % 2;
    drawUI();
    decodeJSON();
  }
}

void getData()
{
  // wait for WiFi connection
  if ((WiFi.status() == WL_CONNECTED))
  {

    HTTPClient http;

    Serial.print("[HTTP] begin...\n");
    Serial.println("http://t.weather.itboy.net/api/weather/city/" + Citycode);
    // configure traged server and url
    http.begin("http://t.weather.itboy.net/api/weather/city/" + Citycode); //HTTP

    Serial.print("[HTTP] GET...\n");
    // start connection and send HTTP header
    int httpCode = http.GET();

    // httpCode will be negative on error
    if (httpCode > 0)
    {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK)
      {
        payload = http.getString();
      }
    }
    else
    {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  }
  else
  {
    Serial.println("Connect Wifi First");
  }
}
void decodeJSON()
{

  forecastCon = 0;
  //JSON解析
  DynamicJsonDocument doc(6144);

  DeserializationError error = deserializeJson(doc, payload);

  if (error)
  {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }

  const char *time = doc["time"]; // "2021-12-11 11: 27: 13"
  Serial.println(time);
  if (modeUI == 0) //当天天气
  {
    JsonObject cityInfo = doc["cityInfo"];
    const char *cityInfo_city = cityInfo["city"];             // "花溪区"
    const char *cityInfo_parent = cityInfo["parent"];         // "贵阳市"
    const char *cityInfo_updateTime = cityInfo["updateTime"]; // "08: 16"
    tft.loadFont("simhei24");
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawString(cityInfo_parent, 38, 216);
    tft.drawString(cityInfo_city, 138, 216);
    tft.drawString(cityInfo_updateTime, 238, 216);
    tft.unloadFont();

    JsonObject data = doc["data"];
    const char *data_shidu = data["shidu"];     // "99%"
    int data_pm25 = data["pm25"];               // 9
    int data_pm10 = data["pm10"];               // 22
    const char *data_quality = data["quality"]; // "优"
    const char *data_wendu = data["wendu"];     // "6"
    const char *data_ganmao = data["ganmao"];   // "各类人群可自由活动"

    for (JsonObject data_forecast_item : data["forecast"].as<JsonArray>())
    {
      const char *data_forecast_item_date = data_forecast_item["date"];       // "11", "12", "13", "14", "15", "16", ...
      const char *data_forecast_item_high = data_forecast_item["high"];       // "高温 9℃", "高温 7℃", "高温 14℃", "高温 ...
      const char *data_forecast_item_low = data_forecast_item["low"];         // "低温 4℃", "低温 3℃", "低温 8℃", "低温 9℃", ...
      const char *data_forecast_item_ymd = data_forecast_item["ymd"];         // "2021-12-11", "2021-12-12", ...
      const char *data_forecast_item_sunrise = data_forecast_item["sunrise"]; // "07: 31", "07: 32", "07: 32", ...
      const char *data_forecast_item_sunset = data_forecast_item["sunset"];   // "18: 02", "18: 02", "18: 03", ...
      int data_forecast_item_aqi = data_forecast_item["aqi"];                 // 55, 21, 28, 32, 31, 24, 17, 16, 31, 48, 54, ...
      const char *data_forecast_item_fx = data_forecast_item["fx"];           // "北风", "北风", "东风", "东南风", "东南风", "北风", ...
      const char *data_forecast_item_fl = data_forecast_item["fl"];           // "2级", "3级", "2级", "2级", "2级", "2级", ...
      const char *data_forecast_item_type = data_forecast_item["type"];       // "小雨", "小雨", "阴", "阴", "阴", "阴", ...

      int length;
      char *data_forecast_item_fx_char;
      length = strlen(data_forecast_item_fx);
      data_forecast_item_fx_char = new char[length + 1];
      strcpy(data_forecast_item_fx_char, data_forecast_item_fx);

      char *data_forecast_item_fl_char;
      length = strlen(data_forecast_item_fl);
      data_forecast_item_fl_char = new char[length + 1];
      strcpy(data_forecast_item_fl_char, data_forecast_item_fl);

      // Serial.println(data_forecast_item_date);
      // Serial.println(data_forecast_item_high);
      // Serial.println(data_forecast_item_low);
      // Serial.println(data_forecast_item_ymd);
      // Serial.println(data_forecast_item_sunrise);
      // Serial.println(data_forecast_item_sunset);
      // Serial.println(data_forecast_item_aqi);
      // Serial.println(data_forecast_item_fx);
      // Serial.println(data_forecast_item_fl);
      // Serial.println(data_forecast_item_type);

      tft.loadFont("qweather-icons50");
      if (strncmp(data_forecast_item_type, "晴", sizeof(data_forecast_item_type)) == 0)
        tft.drawString("\uf101", 35, 46);
      else if (strncmp(data_forecast_item_type, "多云", sizeof(data_forecast_item_type)) == 0)
        tft.drawString("\uf102", 35, 46);
      else if (strncmp(data_forecast_item_type, "阴", sizeof(data_forecast_item_type)) == 0)
        tft.drawString("\uf105", 35, 46);
      else if (strncmp(data_forecast_item_type, "阵雨", sizeof(data_forecast_item_type)) == 0)
        tft.drawString("\uf10a", 35, 46);
      else if (strncmp(data_forecast_item_type, "雷阵雨", sizeof(data_forecast_item_type)) == 0)
        tft.drawString("\uf10c", 35, 46);
      else if (strncmp(data_forecast_item_type, "雨夹雪", sizeof(data_forecast_item_type)) == 0)
        tft.drawString("\uf124", 35, 46);
      else if (strncmp(data_forecast_item_type, "小雨", sizeof(data_forecast_item_type)) == 0)
        tft.drawString("\uf10f", 35, 46);
      else if (strncmp(data_forecast_item_type, "中雨", sizeof(data_forecast_item_type)) == 0)
        tft.drawString("\uf110", 35, 46);
      else if (strncmp(data_forecast_item_type, "大雨", sizeof(data_forecast_item_type)) == 0)
        tft.drawString("\uf111", 35, 46);
      else if (strncmp(data_forecast_item_type, "暴雨", sizeof(data_forecast_item_type)) == 0)
        tft.drawString("\uf112", 35, 46);
      else if (strncmp(data_forecast_item_type, "大暴雨", sizeof(data_forecast_item_type)) == 0)
        tft.drawString("\uf115", 35, 46);
      else if (strncmp(data_forecast_item_type, "特大暴雨", sizeof(data_forecast_item_type)) == 0)
        tft.drawString("\uf116", 35, 46);
      else if (strncmp(data_forecast_item_type, "阵雪", sizeof(data_forecast_item_type)) == 0)
        tft.drawString("\uf12d", 35, 46);
      else if (strncmp(data_forecast_item_type, "小雪", sizeof(data_forecast_item_type)) == 0)
        tft.drawString("\uf120", 35, 46);
      else if (strncmp(data_forecast_item_type, "中雪", sizeof(data_forecast_item_type)) == 0)
        tft.drawString("\uf121", 35, 46);
      else if (strncmp(data_forecast_item_type, "大雪", sizeof(data_forecast_item_type)) == 0)
        tft.drawString("\uf122", 35, 46);
      else if (strncmp(data_forecast_item_type, "暴雪", sizeof(data_forecast_item_type)) == 0)
        tft.drawString("\uf123", 35, 46);
      else if (strncmp(data_forecast_item_type, "小雨-中雨", sizeof(data_forecast_item_type)) == 0)
        tft.drawString("\uf118", 35, 46);
      else if (strncmp(data_forecast_item_type, "中雨-大雨", sizeof(data_forecast_item_type)) == 0)
        tft.drawString("\uf119", 35, 46);
      else if (strncmp(data_forecast_item_type, "大雨-暴雨", sizeof(data_forecast_item_type)) == 0)
        tft.drawString("\uf11a", 35, 46);
      else if (strncmp(data_forecast_item_type, "暴雨-大暴雨", sizeof(data_forecast_item_type)) == 0)
        tft.drawString("\uf11b", 35, 46);
      else if (strncmp(data_forecast_item_type, "大暴雨-特大暴雨", sizeof(data_forecast_item_type)) == 0)
        tft.drawString("\uf11c", 35, 46);
      else if (strncmp(data_forecast_item_type, "小雪-中雪", sizeof(data_forecast_item_type)) == 0)
        tft.drawString("\uf128", 35, 46);
      else if (strncmp(data_forecast_item_type, "中雪-大雪", sizeof(data_forecast_item_type)) == 0)
        tft.drawString("\uf129", 35, 46);
      else if (strncmp(data_forecast_item_type, "大雪-暴雪", sizeof(data_forecast_item_type)) == 0)
        tft.drawString("\uf12a", 35, 46);
      tft.unloadFont();
      tft.loadFont("simhei24");
      tft.drawString(data_forecast_item_type, (120 - tft.textWidth(data_forecast_item_type)) / 2, 106);
      tft.drawString(data_forecast_item_ymd, 0, 162);
      tft.drawString(strcat(data_forecast_item_fx_char, data_forecast_item_fl_char), 120, 190);

      tft.drawString(data_forecast_item_high, 120, 50);
      tft.drawString(data_forecast_item_low, 220, 50);
      //tft.unloadFont();

      break;
    }
    //tft.loadFont("simhei24");
    //tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawString("温度", 120, 22);
    tft.drawString(data_wendu, 220, 22);
    tft.drawString("PM2.5", 120, 78);
    tft.drawNumber(data_pm25, 220, 78);
    tft.drawString("PM10", 120, 106);
    tft.drawNumber(data_pm10, 220, 106);
    tft.drawString("空气质量", 120, 134);
    tft.drawString(data_quality, 220, 134);
    tft.drawString("湿度", 120, 162);
    tft.drawString(data_shidu, 220, 162);
    tft.unloadFont();
  }
  else if (modeUI == 1) //天气预报
  {
    forecastCon = 0;
    JsonObject data = doc["data"];
    for (JsonObject data_forecast_item : data["forecast"].as<JsonArray>())
    {
      if ((forecastCon++) == 0)
      {
        continue;
      }
      const char *data_forecast_item_date = data_forecast_item["date"];       // "11", "12", "13", "14", "15", "16", ...
      const char *data_forecast_item_high = data_forecast_item["high"];       // "高温 9℃", "高温 7℃", "高温 14℃", "高温 ...
      const char *data_forecast_item_low = data_forecast_item["low"];         // "低温 4℃", "低温 3℃", "低温 8℃", "低温 9℃", ...
      const char *data_forecast_item_ymd = data_forecast_item["ymd"];         // "2021-12-11", "2021-12-12", ...
      const char *data_forecast_item_sunrise = data_forecast_item["sunrise"]; // "07: 31", "07: 32", "07: 32", ...
      const char *data_forecast_item_sunset = data_forecast_item["sunset"];   // "18: 02", "18: 02", "18: 03", ...
      int data_forecast_item_aqi = data_forecast_item["aqi"];                 // 55, 21, 28, 32, 31, 24, 17, 16, 31, 48, 54, ...
      const char *data_forecast_item_fx = data_forecast_item["fx"];           // "北风", "北风", "东风", "东南风", "东南风", "北风", ...
      const char *data_forecast_item_fl = data_forecast_item["fl"];           // "2级", "3级", "2级", "2级", "2级", "2级", ...
      const char *data_forecast_item_type = data_forecast_item["type"];       // "小雨", "小雨", "阴", "阴", "阴", "阴", ...

      // Serial.println(data_forecast_item_date);
      // Serial.println(data_forecast_item_high);
      // Serial.println(data_forecast_item_low);
      // Serial.println(data_forecast_item_ymd);
      // Serial.println(data_forecast_item_sunrise);
      // Serial.println(data_forecast_item_sunset);
      // Serial.println(data_forecast_item_aqi);
      // Serial.println(data_forecast_item_fx);
      // Serial.println(data_forecast_item_fl);
      // Serial.println(data_forecast_item_type);
      if (forecastCon == 2 || forecastCon == 4)
      {
        tft.setTextColor(TFT_WHITE, TFT_GREY);
      }
      else
      {
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
      }

      tft.loadFont("qweather-icons50");
      if (strncmp(data_forecast_item_type, "晴", sizeof(data_forecast_item_type)) == 0)
        tft.drawString("\uf101", 5, 4 + (forecastCon - 2) * 80);
      else if (strncmp(data_forecast_item_type, "多云", sizeof(data_forecast_item_type)) == 0)
        tft.drawString("\uf102", 5, 4 + (forecastCon - 2) * 80);
      else if (strncmp(data_forecast_item_type, "阴", sizeof(data_forecast_item_type)) == 0)
        tft.drawString("\uf105", 5, 4 + (forecastCon - 2) * 80);
      else if (strncmp(data_forecast_item_type, "阵雨", sizeof(data_forecast_item_type)) == 0)
        tft.drawString("\uf10a", 5, 4 + (forecastCon - 2) * 80);
      else if (strncmp(data_forecast_item_type, "雷阵雨", sizeof(data_forecast_item_type)) == 0)
        tft.drawString("\uf10c", 5, 4 + (forecastCon - 2) * 80);
      else if (strncmp(data_forecast_item_type, "雨夹雪", sizeof(data_forecast_item_type)) == 0)
        tft.drawString("\uf124", 5, 4 + (forecastCon - 2) * 80);
      else if (strncmp(data_forecast_item_type, "小雨", sizeof(data_forecast_item_type)) == 0)
        tft.drawString("\uf10f", 5, 4 + (forecastCon - 2) * 80);
      else if (strncmp(data_forecast_item_type, "中雨", sizeof(data_forecast_item_type)) == 0)
        tft.drawString("\uf110", 5, 4 + (forecastCon - 2) * 80);
      else if (strncmp(data_forecast_item_type, "大雨", sizeof(data_forecast_item_type)) == 0)
        tft.drawString("\uf111", 5, 4 + (forecastCon - 2) * 80);
      else if (strncmp(data_forecast_item_type, "暴雨", sizeof(data_forecast_item_type)) == 0)
        tft.drawString("\uf112", 5, 4 + (forecastCon - 2) * 80);
      else if (strncmp(data_forecast_item_type, "大暴雨", sizeof(data_forecast_item_type)) == 0)
        tft.drawString("\uf115", 5, 4 + (forecastCon - 2) * 80);
      else if (strncmp(data_forecast_item_type, "特大暴雨", sizeof(data_forecast_item_type)) == 0)
        tft.drawString("\uf116", 5, 4 + (forecastCon - 2) * 80);
      else if (strncmp(data_forecast_item_type, "阵雪", sizeof(data_forecast_item_type)) == 0)
        tft.drawString("\uf12d", 5, 4 + (forecastCon - 2) * 80);
      else if (strncmp(data_forecast_item_type, "小雪", sizeof(data_forecast_item_type)) == 0)
        tft.drawString("\uf120", 5, 4 + (forecastCon - 2) * 80);
      else if (strncmp(data_forecast_item_type, "中雪", sizeof(data_forecast_item_type)) == 0)
        tft.drawString("\uf121", 5, 4 + (forecastCon - 2) * 80);
      else if (strncmp(data_forecast_item_type, "大雪", sizeof(data_forecast_item_type)) == 0)
        tft.drawString("\uf122", 5, 4 + (forecastCon - 2) * 80);
      else if (strncmp(data_forecast_item_type, "暴雪", sizeof(data_forecast_item_type)) == 0)
        tft.drawString("\uf123", 5, 4 + (forecastCon - 2) * 80);
      else if (strncmp(data_forecast_item_type, "小雨-中雨", sizeof(data_forecast_item_type)) == 0)
        tft.drawString("\uf118", 5, 4 + (forecastCon - 2) * 80);
      else if (strncmp(data_forecast_item_type, "中雨-大雨", sizeof(data_forecast_item_type)) == 0)
        tft.drawString("\uf119", 5, 4 + (forecastCon - 2) * 80);
      else if (strncmp(data_forecast_item_type, "大雨-暴雨", sizeof(data_forecast_item_type)) == 0)
        tft.drawString("\uf11a", 5, 4 + (forecastCon - 2) * 80);
      else if (strncmp(data_forecast_item_type, "暴雨-大暴雨", sizeof(data_forecast_item_type)) == 0)
        tft.drawString("\uf11b", 5, 4 + (forecastCon - 2) * 80);
      else if (strncmp(data_forecast_item_type, "大暴雨-特大暴雨", sizeof(data_forecast_item_type)) == 0)
        tft.drawString("\uf11c", 5, 4 + (forecastCon - 2) * 80);
      else if (strncmp(data_forecast_item_type, "小雪-中雪", sizeof(data_forecast_item_type)) == 0)
        tft.drawString("\uf128", 5, 4 + (forecastCon - 2) * 80);
      else if (strncmp(data_forecast_item_type, "中雪-大雪", sizeof(data_forecast_item_type)) == 0)
        tft.drawString("\uf129", 5, 4 + (forecastCon - 2) * 80);
      else if (strncmp(data_forecast_item_type, "大雪-暴雪", sizeof(data_forecast_item_type)) == 0)
        tft.drawString("\uf12a", 5, 4 + (forecastCon - 2) * 80);
      tft.unloadFont();

      int length;
      char *data_forecast_item_fx_char;
      length = strlen(data_forecast_item_fx);
      data_forecast_item_fx_char = new char[length + 1];
      strcpy(data_forecast_item_fx_char, data_forecast_item_fx);

      char *data_forecast_item_fl_char;
      length = strlen(data_forecast_item_fl);
      data_forecast_item_fl_char = new char[length + 1];
      strcpy(data_forecast_item_fl_char, data_forecast_item_fl);

      tft.loadFont("simhei24");
      tft.drawString(data_forecast_item_ymd, 60, 4 + (forecastCon - 2) * 80);
      tft.drawString(data_forecast_item_high, 200, 4 + (forecastCon - 2) * 80);
      tft.drawString(data_forecast_item_sunrise, 60, 30 + (forecastCon - 2) * 80);
      tft.drawString(data_forecast_item_sunset, 60 + tft.textWidth(data_forecast_item_ymd) - tft.textWidth(data_forecast_item_sunset), 30 + (forecastCon - 2) * 80);
      tft.drawString(data_forecast_item_low, 200, 30 + (forecastCon - 2) * 80);
      tft.drawString(data_forecast_item_type, (60 - tft.textWidth(data_forecast_item_type)) / 2, 56 + (forecastCon - 2) * 80);
      tft.drawString(strcat(data_forecast_item_fx_char, data_forecast_item_fl_char), 60, 56 + (forecastCon - 2) * 80);
      tft.drawString("AQI:", 200, 56 + (forecastCon - 2) * 80);
      tft.drawNumber(data_forecast_item_aqi, 260, 56 + (forecastCon - 2) * 80);
      tft.unloadFont();

      if (forecastCon >= 4)
      {
        break;
      }
    }
  }

  ////////////////////////////////////////////////////////////////////////
}
void drawUI()
{
  forecastCon = 0;
  if (modeUI == 1) //天气预报
  {
    tft.fillScreen(TFT_BLACK);
    tft.fillRect(0, 0, 320, 80, TFT_GREY);
    tft.fillRect(0, 160, 320, 80, TFT_GREY);
  }
  else //当天天气
  {
    tft.fillScreen(TFT_BLACK);
  }
}
