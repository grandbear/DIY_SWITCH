#include <TFT_eSPI.h>
#include <SPI.h>
#include "switch.h"
#include <RTClib.h>
#include <ESP32Time.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include "esp_adc_cal.h"
#include "fonts.h"
#include "get_information.h"



String date_information;
String date;
int hour=0,minute=0,second=0;
int year=2023,month=5,day=2;
int heart_x[629],heart_y[629];    //心形坐标存放数组
float radius=10,heartsize=0.3;
int heart_change_flag=0,fill_screen_flag=0;





void attain_time(String url)
{
HTTPClient http;
   if (WiFi.status() == WL_CONNECTED)
    {
      // 设置目标网站的URL
      http.begin(url);

      // 发起GET请求并等待响应
      int httpCode = http.GET();
      
        if (httpCode > 0) 
        {
          if (httpCode == HTTP_CODE_OK)
          {
            // 获取响应的内容
            String response = http.getString();
            DynamicJsonDocument doc(1024);
            DeserializationError error = deserializeJson(doc, response);

            date_information = doc["dateTime"].as<String>();
            date = date_information.substring(0,10);

            year = date_information.substring(0,4).toInt();
            month = date_information.substring(5,7).toInt();
            day = date_information.substring(8,10).toInt();
            hour = date_information.substring(11,13).toInt();
            minute = date_information.substring(14,16).toInt();
            second = date_information.substring(17,19).toInt();
            
          }
        }
          
    }
}



String city;
String weather_condition;
String winddirection;
int tem;

void attain_weather(String url)
{
   HTTPClient http;
   if (WiFi.status() == WL_CONNECTED)
    {
      // 设置目标网站的URL
      http.begin(url);

      // 发起GET请求并等待响应
      int httpCode = http.GET();
      
        if (httpCode > 0) 
        {
          if (httpCode == HTTP_CODE_OK)
          {
            // 获取响应的内容
            String response = http.getString();
            DynamicJsonDocument doc(1024);
            DeserializationError error = deserializeJson(doc, response);

            city = doc["lives"][0]["city"].as<String>();
            weather_condition = doc["lives"][0]["weather"].as<String>();
            tem = doc["lives"][0]["temperature"];
            winddirection = doc["lives"][0]["winddirection"].as<String>();
          }
        }
          
    }
}

int getDayOfWeek(int year, int month, int day)      //根据日期计算今天是周几

{
    int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
    year -= month < 3;
    int dayOfWeek = (year + year/4 - year/100 + year/400 + t[month-1] + day) % 7;
    return dayOfWeek;
}

void draw_heart_shape(void)
{

  //计算心形相关坐标
  int i=0;
  float angle=0;
  int center_x=120,center_y=110;

  // 画爱心
  for (angle = 0; angle < 2.0 * PI; angle += 0.01,i++) 
  {
    heart_x[i] = center_x + radius * 16 * pow(sin(angle), 3) * heartsize;
    heart_y[i] = center_y - radius * (13 * cos(angle) - 5 * cos(2 * angle) - 2 * cos(3 * angle) - cos(4 * angle)) * heartsize;

  }

  for(int i=0;i<629;i++)                                     //边界   第一部分              
  {
    tft.drawPixel(heart_x[i],heart_y[i],TFT_PINK);
  }
    
  for(int k=1;k<8;k++)                                      // 内外层   第二部分
  { 
    delay(200);
    for(int i=0;i<629;i+=k)                                    
    {

      int x1 = random(-5*k,5*k) + heart_x[i] , y1 = random(-5*k,5*k) + heart_y[i];
      int x2 = heart_x[i] , y2 = heart_y[i];

      int x3 = 120 , y3 =  120 - 4*radius*heartsize;
      int x4 = 120 , y4 =  120 + 17*radius*heartsize;
 
      if(pow((x1-x3),2) + pow((y1-y3),2) > 64 && pow((x1-x4),2) + pow((y1-y4),2) > 64 && x1>=0&&x1<=219&&y1>=0&&y1<=219)     //去除顶点周围的噪点
      {
        if(pow((x1-120),2) + pow((y1-120),2) < pow((x2-120),2) + pow((y2-120),2) )
        {
          tft.drawPixel(x1,y1,TFT_PINK);
        }
        else
        {
          tft.drawPixel(x1,y1,TFT_PINK);
        }
      }
      
    }
  }
  
}

int ADC_BAT_PIN = 4;   //GPIO 4 
float bat_voltage=3.5;
uint8_t bat_flag=1;
uint8_t electric_quantities=0;    //锂电池电量

unsigned long previousTime2 = 0;  // 上一次执行任务的时间
unsigned long delayTime2 = 2000;  // 延时时间（毫秒）
unsigned long currentTime2 = millis();  // 获取当前时间

void get_bat_votage(void)
{
  uint32_t sum = 0;        //adc采值
  
  analogReadResolution(12); // 设置ADC位宽为12位
  for (size_t i = 0; i < 50; i++)
  {
    sum += analogRead(ADC_BAT_PIN);
    delay(1);
  }
  sum /= 50;
  bat_voltage = sum * (4.8*4.3 / (4096.0*1.2));        //4.9是根据电阻分压计算
  

  electric_quantities = (sum-680)/3;      //  680-980
  if(electric_quantities>100) electric_quantities=100;   

  if(electric_quantities<10) bat_flag=1;
  else if(electric_quantities>=10&&electric_quantities<25) bat_flag=2;
  else if(electric_quantities>=25&&electric_quantities<50) bat_flag=3;
  else if(electric_quantities>=50&&electric_quantities<75) bat_flag=4;
  else if(electric_quantities>=75) bat_flag=5;






      

  

}







