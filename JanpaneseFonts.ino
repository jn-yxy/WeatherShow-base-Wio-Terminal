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

const char *ssid = "OP";
const char *password = "261020!)@)";
String Citycode = "101260103";
int forecastCon = 0;
String payload;
String jsonStr = "{\"message\":\"success感谢又拍云(upyun.com)提供CDN赞助\",\"status\":200,\"date\":\"20211211\",\"time\":\"2021-12-11 11:27:13\",\"cityInfo\":{\"city\":\"花溪区\",\"citykey\":\"101260103\",\"parent\":\"贵阳市\",\"updateTime\":\"08:16\"},\"data\":{\"shidu\":\"99%\",\"pm25\":9.0,\"pm10\":22.0,\"quality\":\"优\",\"wendu\":\"6\",\"ganmao\":\"各类人群可自由活动\",\"forecast\":[{\"date\":\"11\",\"high\":\"高温 9℃\",\"low\":\"低温 4℃\",\"ymd\":\"2021-12-11\",\"week\":\"星期六\",\"sunrise\":\"07:31\",\"sunset\":\"18:02\",\"aqi\":55,\"fx\":\"北风\",\"fl\":\"2级\",\"type\":\"小雨\",\"notice\":\"雨虽小，注意保暖别感冒\"},{\"date\":\"12\",\"high\":\"高温 7℃\",\"low\":\"低温 3℃\",\"ymd\":\"2021-12-12\",\"week\":\"星期日\",\"sunrise\":\"07:32\",\"sunset\":\"18:02\",\"aqi\":21,\"fx\":\"北风\",\"fl\":\"3级\",\"type\":\"小雨\",\"notice\":\"雨虽小，注意保暖别感冒\"},{\"date\":\"13\",\"high\":\"高温 14℃\",\"low\":\"低温 8℃\",\"ymd\":\"2021-12-13\",\"week\":\"星期一\",\"sunrise\":\"07:32\",\"sunset\":\"18:03\",\"aqi\":28,\"fx\":\"东风\",\"fl\":\"2级\",\"type\":\"阴\",\"notice\":\"不要被阴云遮挡住好心情\"},{\"date\":\"14\",\"high\":\"高温 14℃\",\"low\":\"低温 9℃\",\"ymd\":\"2021-12-14\",\"week\":\"星期二\",\"sunrise\":\"07:33\",\"sunset\":\"18:03\",\"aqi\":32,\"fx\":\"东南风\",\"fl\":\"2级\",\"type\":\"阴\",\"notice\":\"不要被阴云遮挡住好心情\"},{\"date\":\"15\",\"high\":\"高温 15℃\",\"low\":\"低温 9℃\",\"ymd\":\"2021-12-15\",\"week\":\"星期三\",\"sunrise\":\"07:34\",\"sunset\":\"18:03\",\"aqi\":31,\"fx\":\"东南风\",\"fl\":\"2级\",\"type\":\"阴\",\"notice\":\"不要被阴云遮挡住好心情\"},{\"date\":\"16\",\"high\":\"高温 14℃\",\"low\":\"低温 7℃\",\"ymd\":\"2021-12-16\",\"week\":\"星期四\",\"sunrise\":\"07:34\",\"sunset\":\"18:04\",\"aqi\":24,\"fx\":\"北风\",\"fl\":\"2级\",\"type\":\"阴\",\"notice\":\"不要被阴云遮挡住好心情\"},{\"date\":\"17\",\"high\":\"高温 11℃\",\"low\":\"低温 6℃\",\"ymd\":\"2021-12-17\",\"week\":\"星期五\",\"sunrise\":\"07:35\",\"sunset\":\"18:04\",\"aqi\":17,\"fx\":\"东风\",\"fl\":\"2级\",\"type\":\"阴\",\"notice\":\"不要被阴云遮挡住好心情\"},{\"date\":\"18\",\"high\":\"高温 6℃\",\"low\":\"低温 3℃\",\"ymd\":\"2021-12-18\",\"week\":\"星期六\",\"sunrise\":\"07:35\",\"sunset\":\"18:04\",\"aqi\":16,\"fx\":\"北风\",\"fl\":\"3级\",\"type\":\"小雨\",\"notice\":\"雨虽小，注意保暖别感冒\"},{\"date\":\"19\",\"high\":\"高温 10℃\",\"low\":\"低温 3℃\",\"ymd\":\"2021-12-19\",\"week\":\"星期日\",\"sunrise\":\"07:36\",\"sunset\":\"18:05\",\"aqi\":31,\"fx\":\"北风\",\"fl\":\"3级\",\"type\":\"阴\",\"notice\":\"不要被阴云遮挡住好心情\"},{\"date\":\"20\",\"high\":\"高温 13℃\",\"low\":\"低温 4℃\",\"ymd\":\"2021-12-20\",\"week\":\"星期一\",\"sunrise\":\"07:36\",\"sunset\":\"18:05\",\"aqi\":48,\"fx\":\"东北风\",\"fl\":\"2级\",\"type\":\"晴\",\"notice\":\"愿你拥有比阳光明媚的心情\"},{\"date\":\"21\",\"high\":\"高温 15℃\",\"low\":\"低温 7℃\",\"ymd\":\"2021-12-21\",\"week\":\"星期二\",\"sunrise\":\"07:37\",\"sunset\":\"18:06\",\"aqi\":54,\"fx\":\"南风\",\"fl\":\"1级\",\"type\":\"阴\",\"notice\":\"不要被阴云遮挡住好心情\"},{\"date\":\"22\",\"high\":\"高温 17℃\",\"low\":\"低温 8℃\",\"ymd\":\"2021-12-22\",\"week\":\"星期三\",\"sunrise\":\"07:37\",\"sunset\":\"18:06\",\"aqi\":45,\"fx\":\"西南风\",\"fl\":\"2级\",\"type\":\"阴\",\"notice\":\"不要被阴云遮挡住好心情\"},{\"date\":\"23\",\"high\":\"高温 16℃\",\"low\":\"低温 9℃\",\"ymd\":\"2021-12-23\",\"week\":\"星期四\",\"sunrise\":\"07:38\",\"sunset\":\"18:07\",\"aqi\":32,\"fx\":\"东风\",\"fl\":\"2级\",\"type\":\"阴\",\"notice\":\"不要被阴云遮挡住好心情\"},{\"date\":\"24\",\"high\":\"高温 19℃\",\"low\":\"低温 9℃\",\"ymd\":\"2021-12-24\",\"week\":\"星期五\",\"sunrise\":\"07:38\",\"sunset\":\"18:07\",\"aqi\":80,\"fx\":\"南风\",\"fl\":\"4级\",\"type\":\"多云\",\"notice\":\"阴晴之间，谨防紫外线侵扰\"},{\"date\":\"25\",\"high\":\"高温 16℃\",\"low\":\"低温 9℃\",\"ymd\":\"2021-12-25\",\"week\":\"星期六\",\"sunrise\":\"07:39\",\"sunset\":\"18:08\",\"aqi\":59,\"fx\":\"南风\",\"fl\":\"3级\",\"type\":\"小雨\",\"notice\":\"雨虽小，注意保暖别感冒\"}],\"yesterday\":{\"date\":\"10\",\"high\":\"高温 12℃\",\"low\":\"低温 7℃\",\"ymd\":\"2021-12-10\",\"week\":\"星期五\",\"sunrise\":\"07:30\",\"sunset\":\"18:02\",\"aqi\":69,\"fx\":\"东南风\",\"fl\":\"3级\",\"type\":\"阴\",\"notice\":\"不要被阴云遮挡住好心情\"}}}";

uint16_t modeUI = 0;

void getData();
void drawUI();
void decodeJSON();
void setup()
{

  tft.init();
  tft.setRotation(3); //旋转屏幕0-3
  tft.fillScreen(TFT_BLACK);

  // 中文

  while (!SD.begin(SDCARD_SS_PIN, SDCARD_SPI))
  {
    Serial.println("SD card error!\n");
    while (1)
      ;
  }

  delay(1000);
  Serial.begin(115200);

  for (uint8_t t = 4; t > 0; t--)
  {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }
  /*
      WiFi.begin(ssid, password);
      while (WiFi.status() != WL_CONNECTED)
      {
          Serial.print(".");
          // wait 1 second for re-trying
          delay(1000);
      }
      Serial.print("WiFi Connected\n");
  */
  pinMode(WIO_KEY_A, INPUT);
  pinMode(WIO_KEY_B, INPUT);
  drawUI();
}

void loop()
{
  if (0 == digitalRead(WIO_KEY_A))
  {
    while (0 == digitalRead(WIO_KEY_A))
    {
    }
    Serial.println("Begin Get Data");
    //getData();
    payload = jsonStr;
    Serial.println(payload);
    decodeJSON();
  }
  if (0 == digitalRead(WIO_KEY_B))
  {
    while (0 == digitalRead(WIO_KEY_B))
    {
    }
    modeUI = (modeUI + 1) % 2;
    drawUI();
  }
}

void getData()
{
  // wait for WiFi connection
  if ((WiFi.status() == WL_CONNECTED))
  {

    HTTPClient http;

    Serial.print("[HTTP] begin...\n");
    // configure traged server and url
    http.begin("http://t.weather.itboy.net/api/weather/city/101260103"); //HTTP

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
  StaticJsonDocument<384> filter;
  filter["time"] = true;

  JsonObject filter_cityInfo = filter.createNestedObject("cityInfo");
  filter_cityInfo["city"] = true;
  filter_cityInfo["parent"] = true;
  filter_cityInfo["updateTime"] = true;

  JsonObject filter_data = filter.createNestedObject("data");
  filter_data["shidu"] = true;
  filter_data["pm25"] = true;
  filter_data["pm10"] = true;
  filter_data["quality"] = true;
  filter_data["wendu"] = true;
  filter_data["ganmao"] = true;

  JsonObject filter_data_forecast_0 = filter_data["forecast"].createNestedObject();
  filter_data_forecast_0["date"] = true;
  filter_data_forecast_0["high"] = true;
  filter_data_forecast_0["low"] = true;
  filter_data_forecast_0["ymd"] = true;
  filter_data_forecast_0["week"] = false;
  filter_data_forecast_0["sunrise"] = true;
  filter_data_forecast_0["sunset"] = true;
  filter_data_forecast_0["aqi"] = true;
  filter_data_forecast_0["fx"] = true;
  filter_data_forecast_0["fl"] = true;
  filter_data_forecast_0["type"] = true;
  filter_data_forecast_0["notice"] = false;

  DynamicJsonDocument doc(4096);

  DeserializationError error = deserializeJson(doc, payload, DeserializationOption::Filter(filter));

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
    tft.loadFont("simkai24");
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
    tft.loadFont("simkai24");
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawString("温度", 160, 22);
    tft.drawString(data_wendu, 265, 22);
    tft.drawString("PM2.5", 160, 78);
    tft.drawNumber(data_pm25, 265, 78);
    tft.drawString("PM10", 160, 106);
    tft.drawNumber(data_pm10, 265, 106);
    tft.drawString("空气质量", 160, 134);
    tft.drawString(data_quality, 265, 134);
    tft.drawString("湿度", 160, 162);
    tft.drawString(data_shidu, 265, 162);
    tft.unloadFont();

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
      
      tft.loadFont("simkai24");
      tft.drawString(data_forecast_item_high, 160, 50);
      tft.drawString(data_forecast_item_low, 240, 50);
      tft.drawString(data_forecast_item_ymd, 0, 162);
      Serial.println(data_forecast_item_type);
      tft.drawString(data_forecast_item_type, 0, 134);

      tft.unloadFont();

      break;
    }
  }

  ////////////////////////////////////////////////////////////////////////
}
void drawUI()
{
  forecastCon = 0;
  if (modeUI == 1) //天气预报
  {
    tft.fillRect(0, 0, 320, 80, TFT_GREY);
    tft.fillRect(0, 265, 320, 80, TFT_GREY);
  }
  else //当天天气
  {
    tft.fillScreen(TFT_BLACK);
  }
}
