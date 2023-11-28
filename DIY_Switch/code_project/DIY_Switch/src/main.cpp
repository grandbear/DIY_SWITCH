#include <SPI.h>
#include <TFT_eSPI.h>
#include <ArduinoJson.h>
#include "switch.h"
#include "fonts.h"
#include "get_information.h"
#include <HTTPClient.h>
#include "Picture.h"
#include <esp_sleep.h>



#define key 35

//定义两个字符串指针常量
const char* id="Mi 10S";            //wifi名称
const char* psw="xwh123456@";       //wifi密码

uint8_t screen_luminance=50;

void setup() 
{

  pinMode(key, INPUT_PULLDOWN); // 配置唤醒引脚为输入模式，启用下拉电阻

  //初始化屏PWM控制幕亮度为关闭
  sigmaDeltaSetup(5, 5, 5000);
  sigmaDeltaWrite(5, 50);

  tft.init();
  tft.setRotation(0);
  tft.setSwapBytes(true);
  tft.setTextColor(TFT_RED);
  tft.fillScreen(TFT_BLACK);
  WiFi.begin(id,psw);

  while(WiFi.status()!=WL_CONNECTED)      //未连接时阻塞程序，直到连接成功
  {           
    tft.setCursor(90,170,2);
    tft.print("connecting...");
  }

  tft.drawArc(120, 138, 8, 0, 0, 360,TFT_RED,TFT_BLACK);
  tft.drawArc(120, 140, 35, 25, 135, 225,0X3C96,TFT_BLACK);
  tft.drawArc(120, 140, 60, 50, 135, 225,0X3C96,TFT_BLACK);
  tft.drawArc(120, 140, 85, 75, 135, 225,0X3C96,TFT_BLACK);

  tft.fillRect(30, 160, 240, 30, TFT_BLACK);   //局部清除
  tft.setCursor(90,170,2);
  tft.println("connected !");
  tft.setCursor(55,190,2);
  tft.println("getting information...");

  
  attain_weather("https://restapi.amap.com/v3/weather/weatherInfo?city=510700&key=70a0fb18a69178f1f87b506178dc55cb");  //获取天气信息
  attain_time("https://timeapi.io/api/Time/current/zone?timeZone=Asia/Shanghai");           //获取时间信息


  tft.fillScreen(TFT_BLACK);
  tft.setCursor(100,190,2);
  tft.println("loading...");
  draw_heart_shape();
  delay(1000);


  WiFi.mode(WIFI_OFF);           //获取信息后断开网络减小开销
  tft.fillScreen(TFT_BLACK);
  rtc.setTime(second, minute, hour, day, month, year);  //将获取到的时间信息写入内部时钟

  get_bat_votage();     //获取电量

  img.setSwapBytes(true);
  img.createSprite(230, 230);     //屏幕边缘可能不能填充数据,边缘不写入
  img.setTextDatum(4);
  img.setTextColor(TFT_DARKGREY);

  img.fillSprite(TFT_BLACK);  
  img.pushSprite(5, 5);          // 将图像精灵显示在屏幕上的指定位置


  //计算相关刻度坐标
  int b = 0;
  int b2 = 0;

    
    for (int i = 0; i < 360; i++)      
    {
      x[i] = (r * cos(rad * i)) + sx;
      y[i] = (r * sin(rad * i)) + sy;
      px[i] = ((r - 12) * cos(rad * i)) + sx;
      py[i] = ((r - 12) * sin(rad * i)) + sy;

      lx[i] = ((r - 20) * cos(rad * i)) + sx;
      ly[i] = ((r - 20) * sin(rad * i)) + sy;

      if (i % 30 == 0) {
        start[b] = i;
        b++;
      }

      if (i % 6 == 0) {
        startP[b2] = i;
        b2++;
      }
    }

}

void loop()
{
  
  if(brightrn_screen_flag==1&&turn_off_screen_flag!=2) updateWatch();

  if(digitalRead(key)==HIGH&&turn_off_screen_flag==2&&brightrn_screen_flag==0)    
  {
    tft.fillScreen(TFT_BLACK);
    sigmaDeltaWrite(5, screen_luminance);
    tft.setCursor(88,180,2);
    tft.println("awakening...");

    draw_heart_shape();
    get_bat_votage(); 
    brightrn_screen_flag=1;
    turn_off_screen_flag=0;
    times_flag=0;
  }
  
 
}

